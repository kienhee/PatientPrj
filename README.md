# PatientPrj

Chương trình C++17 quản lý khoa và bệnh nhân cho Topic 3. Chạy trong thư mục có `departments.txt` và `patients.txt`. Dữ liệu ban đầu gồm 3 khoa và 4 bệnh nhân theo mục 3.2 của bản thiết kế.

## Chạy trên Windows

Mở `PatientPrj.dev` bằng Embarcadero Dev-C++ rồi nhấn **F11** để biên dịch và chạy. Có thể dùng **F9** để chỉ biên dịch. Khi đã có `PatientPrj.exe`, mở PowerShell trong thư mục `code` và chạy lệnh dưới đây.

```powershell
.\PatientPrj.exe
```

Nếu dùng g++ từ PowerShell, biên dịch cả bảy tệp nguồn trong thư mục `code`.

```powershell
$sources = (Get-ChildItem -File -Filter '*.cpp').FullName
g++ -std=c++17 -Wall -Wextra -Wshadow $sources -o PatientPrj.exe
.\PatientPrj.exe
```

Trên Linux hoặc macOS, chạy `g++ -std=c++17 -Wall -Wextra -Wshadow *.cpp -o PatientPrj` rồi `./PatientPrj`. Khi nhập tên tiếng Việt trên Windows, dùng `chcp 65001` trong cửa sổ terminal trước khi chạy chương trình.

## Thử với dữ liệu mẫu

| Chọn | Thao tác | Kết quả |
|---|---|---|
| `1` | Liệt kê khoa | Cardiology, Pediatrics, Neurology |
| `6` | Liệt kê bệnh nhân theo tên khoa rồi mã | P01, P04, P03, P02 |
| `7` | Nhập `CARD` | P01 và P04 thuộc khoa Cardiology |
| `4` | Đổi mã khoa D01 | Hồ sơ P01 và P04 nhận mã mới. Chọn cả `5` và `11` để lưu liên kết cho lần mở sau. |

Các mục `2`, `3`, `4` lần lượt thêm, tìm và sửa khoa. Các mục `8`, `9`, `10` thêm, xóa và sửa bệnh nhân. Khi sửa, nhấn Enter ở trường có hiển thị giá trị cũ để giữ giá trị đó. Chọn `5` để lưu khoa, `11` để lưu bệnh nhân. Khi chọn `0` và còn thay đổi chưa lưu, nhập `n` để quay lại menu hoặc `y` để bỏ thay đổi và thoát.

## Chạy 12 kịch bản kiểm thử

Trên Windows, nhấp đúp `KiemThu.cmd` để biên dịch lại chương trình rồi chạy toàn bộ kịch bản. Kết quả xuất hiện trên màn hình và trong `tests/ket-qua.txt`.

Sau khi biên dịch, có thể chạy bộ kiểm thử từ PowerShell trong thư mục `code`.

```powershell
.\tests\Test-PatientPrj.ps1
```

Bộ kiểm thử tạo hai tệp dữ liệu trong thư mục tạm, gửi lựa chọn vào chương trình và đối chiếu kết quả trên màn hình lẫn nội dung tệp sau khi khởi động lại. Mỗi dòng `PASS` tương ứng với một kịch bản ở mục 6 của bản thiết kế. Phần kiểm tra bổ sung bao gồm đổi mã khoa có bệnh nhân và đóng luồng nhập giữa lúc sửa hồ sơ.

## Đọc mã nguồn

- `PatientManager.cpp` chứa `main()`, nạp khoa trước bệnh nhân và điều phối menu.
- `Department` và `Patient` kế thừa lớp trừu tượng `Record`. Hàm `printRecord` gọi `screenString()` qua tham chiếu lớp cha. `Patient::operator<` xác định thứ tự sắp xếp.
- `DepartmentList` và `PatientList` quản lý hai `std::vector` và trạng thái chưa lưu. `syncDepartment` cập nhật bản sao khoa trong các bệnh nhân liên quan.
- `Menu`, `InputHelper` và `StringUtils` xử lý việc chọn mục, kiểm tra nhập liệu và chuẩn hóa chuỗi. `RecordID` dùng chung cho việc tra mã và kiểm tra trùng. `RecordFile` đọc và ghi hai định dạng dữ liệu qua `std::ifstream` và `std::ofstream`.
