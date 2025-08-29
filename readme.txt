Xin chào, đây là một chương trình image viewer super tiny
* Tên chương trình A Tiny Image Viewer (cli name: imgv)
* Ngôn ngữ lập trình: C (thư viện stb_image.h)

---

Các chức năng của chương trình bao gồm:
- Mở và hiển thị hình ảnh. (dup click để mở file ảnh, hoặc chạy dưới dạng command line ./imgv <file_path.jpg>)
- Kích thước cửa sổ sẽ được điều chỉnh linh động theo kích thước ảnh. Nếu ảnh quá to (so với kích thước màn hình) sẽ hiển thị maximum.
- Hỗ trợ các định dạng ảnh cơ bản
- Hiển thị tốt đường dẫn/tên file unicode (đặc biệt là tiếng Việt)
- Hỗ trợ khi bấm phím mũi tên <- -> sẽ chuyển sang các ảnh khác cùng thư mục. Cập nhật vị trí cửa sổ để đảm bảo luôn center screen.

