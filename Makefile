
QT_CFLAGS = $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets)

TARGET = imgv
SOURCES = imgv.cpp

# Installation paths
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share
ICONDIR = $(DATADIR)/icons/hicolor
APPLICATIONSDIR = $(DATADIR)/applications


.PHONY: all clean install uninstall check-deps help


all: $(TARGET)

$(TARGET): $(SOURCES)
	g++ $(SOURCES) -o $(TARGET) $(QT_CFLAGS) $(QT_LIBS)




clean:
	rm -f $(TARGET)



install: $(TARGET)
	@echo "Installing imgv..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/
	@echo "✓ Installation complete!"


uninstall:
	@echo "Uninstalling imgv..."
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	@echo "✓ Uninstallation complete!"


check-deps:
	@echo "Checking for Qt5Widgets..."
	@pkg-config --exists Qt5Widgets && echo "✓ Qt5Widgets installed" || echo "✗ Qt5Widgets not installed. Run: sudo dnf install qt5-qtbase-devel"


	@echo "Makefile for imgv (Qt version)"
	@echo ""
	@echo "Available commands:"
	@echo "  make          - Build the program (Qt)"
	@echo "  make clean    - Remove executable"
	@echo "  make install  - Install system-wide (requires sudo)"
	@echo "  make uninstall- Uninstall system-wide (requires sudo)"
	@echo "  make check-deps - Check Qt dependencies"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Required dependencies: Qt5 development libraries, GCC compiler"
	@echo "Install dependencies on Fedora: sudo dnf install gcc-c++ qt5-qtbase-devel"
	@echo "Install dependencies on Ubuntu/Debian: sudo apt install build-essential qtbase5-dev"
