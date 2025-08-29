CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lSDL2 -lm

TARGET = imgv
SOURCES = imgv.c

# Installation paths
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share
ICONDIR = $(DATADIR)/icons/hicolor
APPLICATIONSDIR = $(DATADIR)/applications

.PHONY: all clean install uninstall check-deps help

all: $(TARGET)

$(TARGET): $(SOURCES) stb_image.h stb_image_resize2.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	@echo "Installing imgv..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/
	
	@echo "Installing desktop file..."
	install -d $(DESTDIR)$(APPLICATIONSDIR)
	install -m 644 imgv.desktop $(DESTDIR)$(APPLICATIONSDIR)/
	
	@echo "Installing icon..."
	install -d $(DESTDIR)$(ICONDIR)/scalable/apps
	install -m 644 imgv.svg $(DESTDIR)$(ICONDIR)/scalable/apps/
	
	@echo "Updating desktop database..."
	-update-desktop-database $(DESTDIR)$(APPLICATIONSDIR) 2>/dev/null
	-gtk-update-icon-cache $(DESTDIR)$(ICONDIR) 2>/dev/null
	
	@echo "✓ Installation complete!"

uninstall:
	@echo "Uninstalling imgv..."
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -f $(DESTDIR)$(APPLICATIONSDIR)/imgv.desktop
	rm -f $(DESTDIR)$(ICONDIR)/scalable/apps/imgv.svg
	
	@echo "Updating desktop database..."
	-update-desktop-database $(DESTDIR)$(APPLICATIONSDIR) 2>/dev/null
	-gtk-update-icon-cache $(DESTDIR)$(ICONDIR) 2>/dev/null
	
	@echo "✓ Uninstallation complete!"

# Kiểm tra dependencies
check-deps:
	@echo "Kiểm tra SDL2..."
	@pkg-config --exists sdl2 && echo "✓ SDL2 đã cài đặt" || echo "✗ SDL2 chưa cài đặt. Chạy: sudo apt install libsdl2-dev"

help:
	@echo "Makefile cho A Tiny Image Viewer (imgv)"
	@echo ""
	@echo "Các lệnh có sẵn:"
	@echo "  make          - Biên dịch chương trình"
	@echo "  make clean    - Xóa file thực thi"
	@echo "  make install  - Cài đặt system-wide (cần sudo)"
	@echo "  make uninstall- Gỡ cài đặt system-wide (cần sudo)"
	@echo "  make check-deps - Kiểm tra dependencies"
	@echo "  make help     - Hiển thị help này"
	@echo ""
	@echo "Installation bao gồm:"
	@echo "  - Binary: $(BINDIR)/imgv"
	@echo "  - Desktop file: $(APPLICATIONSDIR)/imgv.desktop"
	@echo "  - Icon: $(ICONDIR)/scalable/apps/imgv.svg"
	@echo ""
	@echo "Dependencies cần thiết:"
	@echo "  - SDL2 development libraries"
	@echo "  - GCC compiler"
	@echo ""
	@echo "Cài đặt dependencies trên Ubuntu/Debian:"
	@echo "  sudo apt update"
	@echo "  sudo apt install build-essential libsdl2-dev"
