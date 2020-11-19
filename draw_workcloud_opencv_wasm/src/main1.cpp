#include <SDL/SDL.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <string>
#include <vector>
#include "Base64.h"

namespace {

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;
SDL_Surface *screen = nullptr;

} // namespace

extern "C" int main(int argc, char **argv) {
  std::cout << "Start main" << std::endl;
  return 0;
}

cv::Mat baseMat;
void setBaseMat(size_t addr, int w, int h) {
  baseMat = cv::Mat(h, w, CV_8UC4, reinterpret_cast<void *>(addr)).clone(); // RGBA
}

class WordLayout{
public:
  WordLayout(std::string word, int x, int y, int w, int h, size_t addr) :
    word(word),
    x(x), y(y), w(w), h(h),
    rgbaImage(cv::Mat(h, w, CV_8UC4, reinterpret_cast<void *>(addr)).clone()),
    dRoi(baseMat(cv::Rect(x, y, w, h)))
  {
    // std::cout << "WordLayout ctor, " << word << "," << x << "," << y << "," << w << "," << h << std::endl;
  }

  std::string word;
  int x, y, w, h;
  cv::Mat rgbaImage;
  cv::Mat dRoi;
};

std::vector<WordLayout> layouts;
void addWordLayout(std::string word, int x, int y, int w, int h, size_t addr) {
  layouts.push_back(WordLayout(word, x, y, w, h, addr));
}

void doOpenCvTask(int width, int height) {
  auto start1 = std::chrono::system_clock::now();
  if (screen == nullptr) {
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
  }
  auto end1 = std::chrono::system_clock::now();
  std::chrono::duration<double, std::micro> duration1 = end1 - start1;
  std::cout << "cpp step1(SDL Init): " << std::to_string(duration1.count() / 1000.0) << "ms" << std::endl;

  // std::cout << "Start doOpenCvTask" << std::endl;
  // std::cout << "start drawing" << std::endl;

  auto start2 = std::chrono::system_clock::now();
  for(auto it = std::begin(layouts); it != std::end(layouts); ++it){
    // std::cout << it->x << "," << it->y << "," << it->w << "," << it->h << std::endl;
    cv::max(it->rgbaImage, it->dRoi, it->dRoi);
  }
  auto end2 = std::chrono::system_clock::now();

  std::chrono::duration<double, std::micro> duration2 = end2 - start2;
  std::cout << "cpp step2(process): " << std::to_string(duration2.count() / 1000.0) << "ms" << std::endl;

  auto start3 = std::chrono::system_clock::now();
  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);
  auto end31 = std::chrono::system_clock::now();
  cv::Mat dstRGBAImage(height, width, CV_8UC4, screen->pixels);
  baseMat.copyTo(dstRGBAImage);
  auto end32 = std::chrono::system_clock::now();
  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);
  auto end33 = std::chrono::system_clock::now();
  SDL_Flip(screen);
  auto end34 = std::chrono::system_clock::now();

  std::chrono::duration<double, std::micro> duration31 = end31 - start3;
  std::cout << "cpp step3(lock): " << std::to_string(duration31.count() / 1000.0) << "ms" << std::endl;
  std::chrono::duration<double, std::micro> duration32 = end32 - end31;
  std::cout << "cpp step3(draw): " << std::to_string(duration32.count() / 1000.0) << "ms" << std::endl;
  std::chrono::duration<double, std::micro> duration33 = end33 - end32;
  std::cout << "cpp step3(unlock): " << std::to_string(duration33.count() / 1000.0) << "ms" << std::endl;
  std::chrono::duration<double, std::micro> duration34 = end34 - end33;
  std::cout << "cpp step3(flip): " << std::to_string(duration34.count() / 1000.0) << "ms" << std::endl;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("doOpenCvTask", &doOpenCvTask);
  emscripten::function("addWordLayout", &addWordLayout);
  emscripten::function("setBaseMat", &setBaseMat);
}