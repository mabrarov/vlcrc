#include <cstdlib>
#include <memory>
#include <chrono>
#include <thread>
#include <vlc/vlc.h>

int main(int argc, char* argv[])
{
  typedef std::unique_ptr<libvlc_instance_t, void (*)(libvlc_instance_t*)>
      libvlc_instance_ptr;
  typedef std::unique_ptr<libvlc_media_t, void (*)(libvlc_media_t*)>
      libvlc_media_ptr;
  typedef std::unique_ptr<libvlc_media_player_t,
      void (*)(libvlc_media_player_t*)> libvlc_media_player_ptr;

  // Load the VLC engine
  libvlc_instance_ptr libvlc_instance(libvlc_new(0, nullptr), libvlc_release);

  // Create a new item
  libvlc_media_ptr media(libvlc_media_new_path(libvlc_instance.get(), argv[1]),
      libvlc_media_release);

  // Create a media player playing environment
  libvlc_media_player_ptr media_player
      (libvlc_media_player_new_from_media(media.get()),
          libvlc_media_player_release);

  // Media is not needed anymore
  media.reset();

  // Turn on fullscreen mode
  libvlc_toggle_fullscreen(media_player.get());

  // Start playing
  libvlc_media_player_play(media_player.get());

  // Let it play a bit
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Stop playing
  libvlc_media_player_stop(media_player.get());

  return EXIT_SUCCESS;
}
