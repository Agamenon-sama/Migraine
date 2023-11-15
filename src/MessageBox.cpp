#include "MessageBox.h"

#include <slog/slog.h>

#include <SDL2/SDL.h>

void MessageBox::info(const std::string &message, const std::string &title) {
    slog::info(title);
    slog::message(message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), nullptr);
}
void MessageBox::warning(const std::string &message, const std::string &title) {
    slog::warning(title);
    slog::message(message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), nullptr);
}
void MessageBox::error(const std::string &message, const std::string &title) {
    slog::error(title);
    slog::message(message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), nullptr);
}