# A Tiny Image Viewer (imgv)

Một trình xem ảnh siêu nhỏ gọn và nhanh chóng cho Linux.

## ✨ Tính năng

- **⚡ Siêu nhanh**: Tải và hiển thị ảnh ngay lập tức
- **🔄 Navigation đơn giản**: Phím mũi tên để chuyển ảnh
- **📐 Auto-resize thông minh**: Tự động điều chỉnh theo kích thước ảnh
- **🖼️ Hỗ trợ đa định dạng**: JPG, PNG, BMP, TGA, GIF
- **🌐 Unicode support**: Hiển thị tên file tiếng Việt
- **🎯 Tự động căn giữa**: Window luôn ở vị trí thuận tiện

## �️ Cài đặt

### Yêu cầu hệ thống
- Linux (tested on Fedora)
- SDL2 development libraries
- GCC compiler

### Cài đặt dependencies

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc SDL2-devel
```

**Arch Linux:**
```bash
sudo pacman -S gcc sdl2
```

### Biên dịch và cài đặt

```bash
# Clone hoặc download source code
cd imgv/

# Biên dịch
make

# Cài đặt system-wide (tùy chọn)
sudo make install
```

### Gỡ cài đặt

```bash
sudo make uninstall
```

## 🚀 Sử dụng

### Command line
```bash
# Xem một ảnh
./imgv image.jpg

# Xem ảnh trong thư mục
./imgv /path/to/images/

# Sau khi cài đặt system-wide
imgv image.jpg
```

### Desktop Integration

Sau khi `make install`, imgv sẽ xuất hiện trong:
- **Applications menu** của desktop environment
- **Right-click → Open with** cho image files
- **Default application** cho image types (có thể thiết lập trong settings)

### ⌨️ Phím tắt
- **←** hoặc **Backspace**: Ảnh trước
- **→** hoặc **Space**: Ảnh tiếp theo  
- **Esc** hoặc **Q**: Thoát

## 📁 Cấu trúc dự án

```
imgv/
├── imgv.c              # Source code chính
├── imgv.svg            # Application icon
├── imgv.desktop        # Desktop integration file
├── Makefile           # Build và installation script
├── stb_image.h        # Image loading library
├── stb_image_resize2.h # Image resizing library
└── README.md          # Documentation
```

## 🔧 Tính năng kỹ thuật

- **Single window architecture**: Hiệu quả và ổn định
- **Smart memory management**: Tự động giải phóng bộ nhớ
- **Cross-platform compatibility**: Hoạt động trên mọi Linux distro
- **Minimal dependencies**: Chỉ cần SDL2

## 📄 License

GPL v3 - See original Vietnamese readme.txt for details.er (imgv) - v1.6

Một trình xem ảnh siêu nhỏ gọn và nhanh chóng, được tối ưu hóa đặc biệt cho Linux desktop environments.

## ✨ Tính năng

- **🎯 Perfect Window Centering**: Luôn center chính xác trên mọi desktop environment
- **� Zero Taskbar Flicker**: Hoàn toàn không có hiện tượng "cà giật cà giật" trên GNOME
- **⚡ Siêu nhanh**: Tải và hiển thị ảnh ngay lập tức
- **🔄 Navigation mượt mà**: Phím mũi tên để chuyển ảnh
- **📐 Auto-resize thông minh**: Tự động điều chỉnh theo kích thước ảnh
- **🖼️ Hỗ trợ đa định dạng**: JPG, PNG, BMP, TGA, GIF
- **🌐 Unicode support**: Hiển thị tên file tiếng Việt hoàn hảo

## 🔧 Technical Highlights (v1.6)

- **Window Recreation Architecture**: Sử dụng SDL_WINDOWPOS_CENTERED cho perfect positioning
- **GNOME Optimized**: Đặc biệt tối ưu cho Fedora Linux + GNOME
- **Professional Desktop Integration**: Zero flicker, smooth transitions
