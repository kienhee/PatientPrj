param(
    [string]$Program = (Join-Path (Split-Path $PSScriptRoot -Parent) 'PatientPrj.exe')
)

$ErrorActionPreference = 'Stop'
$executable = (Resolve-Path -LiteralPath $Program).Path
$utf8 = [Text.UTF8Encoding]::new($false)
$tempRoot = (Get-Item -LiteralPath ([IO.Path]::GetTempPath())).FullName.TrimEnd('\', '/')
$caseFolder = 'PatientPrj-' + [guid]::NewGuid().ToString('N')
$testDir = Join-Path $tempRoot $caseFolder
[void][IO.Directory]::CreateDirectory($testDir)
$deptFile = Join-Path $testDir 'departments.txt'
$patientFile = Join-Path $testDir 'patients.txt'
$departments = @('D01,Cardiology,2,250000', 'D02,Pediatrics,3,180000', 'D03,Neurology,4,320000')
$patients = @('P01,D01,Nguyen An,Hypertension,52', 'P02,D02,Tran Binh,Fever,9',
              'P03,D03,Le Chi,Migraine,34', 'P04,D01,Pham Dung,Arrhythmia,61')

function Run-App([string[]]$Keys) {
    $start = [Diagnostics.ProcessStartInfo]::new()
    $start.FileName = $executable
    $start.WorkingDirectory = $testDir
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardInput = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $start.StandardOutputEncoding = $utf8
    $process = [Diagnostics.Process]::Start($start)
    try {
        $stdout = $process.StandardOutput.ReadToEndAsync()
        $stderr = $process.StandardError.ReadToEndAsync()
        $inputBytes = $utf8.GetBytes(($Keys -join [Environment]::NewLine) + [Environment]::NewLine)
        $process.StandardInput.BaseStream.Write($inputBytes, 0, $inputBytes.Length)
        $process.StandardInput.Close()
        if (!$process.WaitForExit(8000)) {
            $process.Kill()
            throw 'Program did not exit within 8 seconds.'
        }
        if ($process.ExitCode -ne 0) { throw "Program exited with $($process.ExitCode). $($stderr.Result)" }
        return $stdout.Result
    } finally {
        $process.Dispose()
    }
}

function Check([bool]$Condition, [string]$Message) {
    if (!$Condition) { throw $Message }
}

function Expect([string]$Output, [string[]]$Texts) {
    foreach ($phrase in $Texts) {
        Check ($Output.Contains($phrase)) "Missing output '$phrase'."
    }
}

function Times([string]$Output, [string]$Phrase, [int]$Expected) {
    $actual = [regex]::Matches($Output, [regex]::Escape($Phrase)).Count
    Check ($actual -eq $Expected) "Expected '$Phrase' $Expected time(s), found $actual."
}

function PatientOrder([string]$Output, [string]$Order) {
    $matchesFound = [regex]::Matches($Output, '(?m)^(P\d+)\s+')
    $ids = @($matchesFound | ForEach-Object { $_.Groups[1].Value }) -join ','
    Check ($ids -eq $Order) "Expected patient order '$Order', found '$ids'."
}

try {
    [IO.File]::WriteAllLines($deptFile, $departments, $utf8)
    [IO.File]::WriteAllLines($patientFile, $patients, $utf8)

    $out = Run-App @('1', '0')
    Expect $out @('Loaded 3 departments (0 line(s) skipped).', 'Loaded 4 patients',
                  'Cardiology', 'Pediatrics', 'Neurology', 'Total: 3')
    '01 PASS - load and list departments'

    $out = Run-App @('2', 'd01', 'D04', '', 'Ortho,pedics', 'Orthopedics', '5',
                     'abc', '-5', 'inf', '280000', '1', '5', '0')
    Expect $out @('Department ID already exists.', 'Value must not be empty.',
                  "Value must not contain ','.", 'Department added.', 'Total: 4')
    Times $out 'Service fee must be a number greater than 0.' 3
    '02 PASS - validate and add a department'

    $out = Run-App @('3', ' d02 ', '3', 'D99', '0')
    Expect $out @('Pediatrics', 'Department not found.')
    '03 PASS - department lookup with existing and missing IDs'

    $out = Run-App @('4', 'D02', 'D01', '', 'Pediatric Care', '', '0', '200000', '6', '5', '0')
    Expect $out @('Department ID already exists.', 'Service fee must be a number greater than 0.',
                  'Department updated.', 'Saved 4 departments')
    Check ($out -match 'P02\s+D02\s+Pediatric Care\s+Tran Binh') 'Patient department copy was not refreshed.'
    '04 PASS - update department and synchronize patient display'

    $deptLines = [IO.File]::ReadAllLines($deptFile, $utf8)
    Check ($deptLines.Length -eq 4 -and $deptLines[1] -ceq 'D02,Pediatric Care,3,200000') 'Unexpected departments.txt.'
    $out = Run-App @('1', '0')
    Expect $out @('Loaded 4 departments', 'Pediatric Care', 'Orthopedics', 'Total: 4')
    '05 PASS - save and reload departments'

    $out = Run-App @('6', '11', '0')
    PatientOrder $out 'P01,P04,P03,P02'
    Check (([IO.File]::ReadAllLines($patientFile, $utf8) -join '|') -ceq ($patients -join '|')) 'Listing changed file order.'
    '06 PASS - sort by department and patient ID, preserve storage order'

    PatientOrder (Run-App @('7', 'olog', '0')) 'P01,P04,P03'
    PatientOrder (Run-App @('7', 'CARD', '0')) 'P01,P04'
    Expect (Run-App @('7', 'surgery', '0')) @('No patient found.')
    '07 PASS - partial department name, mixed case and no match'

    $out = Run-App @('8', 'p01', 'P05', '9', 'abc', '4', 'Vo Giang', 'Fracture',
                     '-3', '12abc', '40', '11', '0')
    Expect $out @('Patient ID already exists.', 'Patient added.')
    Times $out 'Invalid choice.' 2
    Times $out 'Age must be a whole number >= 0.' 2
    Expect ([IO.File]::ReadAllText($patientFile)) @('P05,D04,Vo Giang,Fracture,40')
    '08 PASS - validate and add a patient from the department menu'

    $out = Run-App @('9', 'P99', '9', 'P05', 'n', '9', 'p05', 'Y', '11', '0')
    Expect $out @('Patient not found.', 'Cancelled.', 'Patient removed.')
    Check (!([IO.File]::ReadAllText($patientFile).Contains('P05,'))) 'Removed patient still present.'
    '09 PASS - cancel, confirm and persist deletion'

    $out = Run-App @('10', 'P99', '10', 'P02', 'P01', '', 'y', '1', '', '', '-1', '10', '11', '0')
    Expect $out @('Patient not found.', 'Patient ID already exists.', 'Patient updated.',
                  'Age must be a whole number >= 0.')
    Check ($out -match 'P02\s+D01\s+Cardiology\s+Tran Binh\s+Fever\s+10') 'Updated patient row is incorrect.'
    '10 PASS - update patient, change department and keep optional fields'

    Expect ([IO.File]::ReadAllText($patientFile)) @('P02,D01,Tran Binh,Fever,10')
    $out = Run-App @('6', '0')
    PatientOrder $out 'P01,P02,P04,P03'
    Check ($out -match 'P02\s+D01\s+Cardiology\s+Tran Binh\s+Fever\s+10') 'Saved patient did not reload.'
    '11 PASS - save and reload patients'

    [IO.File]::Move($deptFile, (Join-Path $testDir 'departments.backup'))
    $out = Run-App @('1', '8', '6', '0')
    Expect $out @('Cannot open file: departments.txt', 'No department.', 'No patient.',
                  'department D01 does not exist', 'No department available. Please add a department first.')
    [IO.File]::Move((Join-Path $testDir 'departments.backup'), $deptFile)
    [IO.File]::AppendAllLines($deptFile, [string[]]@('D05,Test,1,abc', '', 'D01,Duplicate,3,9', 'X,A,2', 'Z,A,2,', 'X,A,2,0'), $utf8)
    [IO.File]::AppendAllLines($patientFile, [string[]]@('P09,D99,X,Y,1', 'p01,D02,X,Y,1', 'P10,D01,X,Y,12.5', 'P11,D02,X,Y,-2'), $utf8)
    $before = [IO.File]::ReadAllText($deptFile)
    $out = Run-App @('abc', '12', '2', 'D06', 'Test', 'B1', '+300', '0', 'n', '0', 'y')
    Expect $out @("departments.txt line 5: invalid serviceFee 'abc'", 'departments.txt line 7: duplicate ID D01',
                  'departments.txt line 8: expected 4 fields, got 3', 'departments.txt line 9: empty field',
                  'departments.txt line 10: serviceFee must be > 0', 'patients.txt line 5: department D99 does not exist',
                  'patients.txt line 6: duplicate ID P01', "patients.txt line 7: invalid age '12.5'",
                  'patients.txt line 8: age must not be negative', 'Loaded 4 departments (5 line(s) skipped).')
    Times $out 'Invalid choice.' 2
    Times $out 'departments.txt has unsaved changes.' 2
    Check ([IO.File]::ReadAllText($deptFile) -ceq $before) 'Exit wrote to departments.txt.'
    '12 PASS - missing files, bad rows, invalid menu and exit without saving'

    [IO.File]::WriteAllLines($deptFile, $departments, $utf8)
    [IO.File]::WriteAllLines($patientFile, $patients, $utf8)
    $out = Run-App @('4', 'd01', 'd07', '', '', '', '0', 'n', '5', '11', '0')
    Expect $out @('2 patient(s) updated. Save both departments.txt (5) and patients.txt (11).',
                  'departments.txt has unsaved changes.', 'patients.txt has unsaved changes.')
    Expect ([IO.File]::ReadAllText($patientFile)) @('P01,D07,', 'P04,D07,')
    PatientOrder (Run-App @('6', '0')) 'P01,P04,P03,P02'
    $before = [IO.File]::ReadAllText($patientFile)
    Expect (Run-App @('10', 'P01', 'P09', 'n', 'New name')) @('Input closed. Exiting without saving.')
    Check ([IO.File]::ReadAllText($patientFile) -ceq $before) 'EOF wrote a partial update.'
    'Extra PASS - propagate renamed department ID to both files and exit cleanly on EOF'
} finally {
    $privateDir = Get-Item -LiteralPath $testDir
    if ($privateDir.Parent.FullName.TrimEnd('\', '/') -eq $tempRoot -and $privateDir.Name -eq $caseFolder) {
        Remove-Item -LiteralPath $privateDir.FullName -Recurse -Force
    }
}
