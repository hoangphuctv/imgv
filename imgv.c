#define _GNU_SOURCE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    char **files;
    int count;
    int current;
} ImageList;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int img_width, img_height;
    int win_width, win_height;
    ImageList image_list;
    char current_dir[1024];
} ImageViewer;

// Always center on primary display for consistent behavior
int get_current_display() {
    // For simplicity and consistency, always use primary display
    // This avoids mouse position detection issues across different terminals
    return 0; // Primary display
}

// Hàm resize và center cửa sổ
int resize_and_center_window(ImageViewer *viewer, const char *title) {
    if (!viewer->window) {
        // Tìm màn hình hiện tại (luôn dùng primary display)
        int current_display = get_current_display();
        
        // Lấy thông tin màn hình hiện tại
        SDL_Rect display_bounds;
        if (SDL_GetDisplayBounds(current_display, &display_bounds) != 0) {
            // Fallback to primary display
            SDL_GetDisplayBounds(0, &display_bounds);
        }
        
        // Tính toán vị trí center của màn hình hiện tại
        int pos_x = display_bounds.x + (display_bounds.w - viewer->win_width) / 2;
        int pos_y = display_bounds.y + (display_bounds.h - viewer->win_height) / 2;
        
        // Tạo cửa sổ tại vị trí đã tính toán với consistent flags
        viewer->window = SDL_CreateWindow(title, pos_x, pos_y,
                                        viewer->win_width, viewer->win_height, 
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
        if (!viewer->window) {
            printf("Không thể tạo cửa sổ: %s\n", SDL_GetError());
            return 0;
        }
        
        // Đảm bảo window được positioned đúng trong mọi trường hợp
        SDL_SetWindowPosition(viewer->window, pos_x, pos_y);
        
        // Force window to be non-resizable để consistent decoration
        SDL_SetWindowResizable(viewer->window, SDL_FALSE);
        
        viewer->renderer = SDL_CreateRenderer(viewer->window, -1, SDL_RENDERER_ACCELERATED);
        if (!viewer->renderer) {
            printf("Không thể tạo renderer: %s\n", SDL_GetError());
            return 0;
        }
        
        return 1;
    }
    
    // Cập nhật tiêu đề và kích thước cửa sổ
    SDL_SetWindowTitle(viewer->window, title);
    SDL_SetWindowSize(viewer->window, viewer->win_width, viewer->win_height);
    
    // Đảm bảo consistent window properties
    SDL_SetWindowResizable(viewer->window, SDL_FALSE);
    
    SDL_PumpEvents();
    
    return 1;
}

// Kiểm tra xem file có phải là ảnh không
int is_image_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    
    return (strcasecmp(ext, ".jpg") == 0 ||
            strcasecmp(ext, ".jpeg") == 0 ||
            strcasecmp(ext, ".png") == 0 ||
            strcasecmp(ext, ".bmp") == 0 ||
            strcasecmp(ext, ".tga") == 0 ||
            strcasecmp(ext, ".gif") == 0);
}

// Lấy danh sách file ảnh trong thư mục
void load_image_list(ImageViewer *viewer, const char *filepath) {
    char *dir_path = strdup(filepath);
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        strcpy(viewer->current_dir, dir_path);
    } else {
        strcpy(viewer->current_dir, ".");
    }
    
    DIR *dir = opendir(viewer->current_dir);
    if (!dir) {
        free(dir_path);
        return;
    }
    
    // Đếm số file ảnh
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (is_image_file(entry->d_name)) {
            count++;
        }
    }
    
    rewinddir(dir);
    
    // Cấp phát bộ nhớ và lưu danh sách
    viewer->image_list.files = malloc(count * sizeof(char*));
    viewer->image_list.count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (is_image_file(entry->d_name)) {
            viewer->image_list.files[viewer->image_list.count] = strdup(entry->d_name);
            
            // Kiểm tra file hiện tại
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", viewer->current_dir, entry->d_name);
            if (strcmp(full_path, filepath) == 0) {
                viewer->image_list.current = viewer->image_list.count;
            }
            
            viewer->image_list.count++;
        }
    }
    
    closedir(dir);
    free(dir_path);
}

// Giải phóng danh sách ảnh
void free_image_list(ImageList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->files[i]);
    }
    free(list->files);
    list->files = NULL;
    list->count = 0;
}

// Tải và hiển thị ảnh
int load_image(ImageViewer *viewer, const char *filepath) {
    unsigned char *img_data = stbi_load(filepath, &viewer->img_width, &viewer->img_height, NULL, 4);
    if (!img_data) {
        printf("Không thể tải ảnh: %s\n", filepath);
        return 0;
    }
    
    // Tính toán kích thước cửa sổ phù hợp
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    int screen_w = dm.w * 0.9; // 90% màn hình
    int screen_h = dm.h * 0.9;
    
    viewer->win_width = viewer->img_width;
    viewer->win_height = viewer->img_height;
    
    // Nếu ảnh quá lớn, resize
    if (viewer->win_width > screen_w || viewer->win_height > screen_h) {
        float scale_w = (float)screen_w / viewer->img_width;
        float scale_h = (float)screen_h / viewer->img_height;
        float scale = (scale_w < scale_h) ? scale_w : scale_h;
        
        viewer->win_width = (int)(viewer->img_width * scale);
        viewer->win_height = (int)(viewer->img_height * scale);
        
        // Resize ảnh
        unsigned char *resized_data = malloc(viewer->win_width * viewer->win_height * 4);
        stbir_resize_uint8_srgb(img_data, viewer->img_width, viewer->img_height, 0,
                              resized_data, viewer->win_width, viewer->win_height, 0, 4);
        stbi_image_free(img_data);
        img_data = resized_data;
    }
    
    // Tạo texture
    if (viewer->texture) {
        SDL_DestroyTexture(viewer->texture);
    }
    
    // Tạo title với tên file
    char title[1024];
    const char *filename = strrchr(filepath, '/');
    filename = filename ? filename + 1 : filepath;
    snprintf(title, sizeof(title), "imgv - %s (%dx%d)", filename, viewer->img_width, viewer->img_height);
    
    // Resize và center cửa sổ (không recreate để tránh taskbar flicker)
    if (!resize_and_center_window(viewer, title)) {
        free(img_data);
        return 0;
    }
    
    // Tạo texture mới
    viewer->texture = SDL_CreateTexture(viewer->renderer, SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_STATIC, viewer->win_width, viewer->win_height);
    
    SDL_UpdateTexture(viewer->texture, NULL, img_data, viewer->win_width * 4);
    
    free(img_data);
    return 1;
}

// Chuyển sang ảnh tiếp theo
void next_image(ImageViewer *viewer) {
    if (viewer->image_list.count == 0) return;
    
    viewer->image_list.current = (viewer->image_list.current + 1) % viewer->image_list.count;
    
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", viewer->current_dir, 
             viewer->image_list.files[viewer->image_list.current]);
    
    load_image(viewer, filepath);
}

// Chuyển sang ảnh trước đó
void prev_image(ImageViewer *viewer) {
    if (viewer->image_list.count == 0) return;
    
    viewer->image_list.current = (viewer->image_list.current - 1 + viewer->image_list.count) % viewer->image_list.count;
    
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", viewer->current_dir, 
             viewer->image_list.files[viewer->image_list.current]);
    
    load_image(viewer, filepath);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Sử dụng: %s <đường_dẫn_ảnh>\n", argv[0]);
        return 1;
    }
    
    // Detach from terminal - fork to background
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }
    
    if (pid > 0) {
        // Parent process - exit immediately to return control to terminal
        return 0;
    }
    
    // Child process continues - detach from terminal session
    if (setsid() < 0) {
        perror("setsid failed");
        return 1;
    }
    
    // Force consistent window decorations
    setenv("SDL_VIDEO_WAYLAND_WMCLASS", "imgv", 1);
    setenv("SDL_VIDEO_X11_WMCLASS", "imgv", 1);
    // Disable problematic libdecor to get consistent borders
    setenv("SDL_VIDEO_WAYLAND_PREFER_LIBDECOR", "0", 1);
    
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Không thể khởi tạo SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    // Set SDL hints để tối ưu cho GNOME/Wayland
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "1");
    SDL_SetHint(SDL_HINT_VIDEO_X11_WINDOW_VISUALID, "");
    
    ImageViewer viewer = {0};
    
    // Không cần main window nữa, chỉ dùng một cửa sổ duy nhất
    viewer.window = NULL;
    viewer.renderer = NULL;
    
    // Tải danh sách ảnh trong thư mục
    load_image_list(&viewer, argv[1]);
    
    // Tải ảnh đầu tiên (sẽ tạo cửa sổ)
    if (!load_image(&viewer, argv[1])) {
        printf("Không thể tải ảnh: %s\n", argv[1]);
        if (viewer.renderer) SDL_DestroyRenderer(viewer.renderer);
        if (viewer.window) SDL_DestroyWindow(viewer.window);
        SDL_Quit();
        return 1;
    }
    
    // Vòng lặp chính
    SDL_Event event;
    int running = 1;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            running = 0;
                            break;
                            
                        case SDLK_RIGHT:
                        case SDLK_SPACE:
                            next_image(&viewer);
                            break;
                            
                        case SDLK_LEFT:
                        case SDLK_BACKSPACE:
                            prev_image(&viewer);
                            break;
                    }
                    break;
            }
        }
        
        // Render
        SDL_SetRenderDrawColor(viewer.renderer, 0, 0, 0, 255);
        SDL_RenderClear(viewer.renderer);
        
        if (viewer.texture) {
            SDL_RenderCopy(viewer.renderer, viewer.texture, NULL, NULL);
        }
        
        SDL_RenderPresent(viewer.renderer);
        
        SDL_Delay(16); // ~60 FPS
    }
    
    // Dọn dẹp
    if (viewer.texture) {
        SDL_DestroyTexture(viewer.texture);
    }
    free_image_list(&viewer.image_list);
    
    // Dọn dẹp cửa sổ
    if (viewer.renderer) {
        SDL_DestroyRenderer(viewer.renderer);
    }
    if (viewer.window) {
        SDL_DestroyWindow(viewer.window);
    }
    
    SDL_Quit();
    
    return 0;
}
