#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vlc/vlc.h>

int main(int argc, char* argv[])
{
  try
  {
    typedef std::unique_ptr<libvlc_instance_t, void (*)(libvlc_instance_t*)>
        libvlc_instance_ptr;
    typedef std::unique_ptr<libvlc_media_t, void (*)(libvlc_media_t*)>
        libvlc_media_ptr;
    typedef std::unique_ptr<libvlc_media_player_t,
        void (*)(libvlc_media_player_t*)> libvlc_media_player_ptr;

    // Load the VLC engine
    libvlc_instance_t* libvlc_raw_instance = libvlc_new(0, nullptr);
    if (!libvlc_raw_instance)
    {
      throw std::runtime_error("Failed to initialize LibVLC");
    }
    libvlc_instance_ptr libvlc_instance(libvlc_raw_instance, libvlc_release);

    // Create a new item
    libvlc_media_t
        * raw_media = libvlc_media_new_path(libvlc_instance.get(), argv[1]);
    if (!raw_media)
    {
      throw std::runtime_error(
          "Failed to load media: " + std::string(libvlc_errmsg()));
    }
    libvlc_media_ptr media(raw_media, libvlc_media_release);

    // Create a media player playing environment
    libvlc_media_player_t
        * raw_media_player = libvlc_media_player_new(libvlc_instance.get());
    if (!raw_media_player)
    {
      throw std::runtime_error(
          "Failed to create media player: " + std::string(libvlc_errmsg()));
    }
    libvlc_media_player_ptr
        media_player(raw_media_player, libvlc_media_player_release);

    libvlc_event_manager_t
        * event_manager = libvlc_media_player_event_manager(media_player.get());

    struct wait_data_type
    {
      typedef std::mutex mutex_type;
      typedef std::lock_guard<mutex_type> lock_guard_type;
      typedef std::unique_lock<mutex_type> unique_lock_type;

      mutex_type mutex;
      std::condition_variable condition;
      bool finished_playing;
    } wait_data;

    libvlc_callback_t end_reached_callback =
        [](const struct libvlc_event_t* event, void* data)
        {
          if (libvlc_MediaPlayerEndReached == event->type)
          {
            wait_data_type* wait_data = reinterpret_cast<wait_data_type*>(data);
            wait_data_type::lock_guard_type lock(wait_data->mutex);
            wait_data->finished_playing = true;
            wait_data->condition.notify_all();
          }
        };

    if (libvlc_event_attach(event_manager, libvlc_MediaPlayerEndReached,
        end_reached_callback, &wait_data))
    {
      throw std::runtime_error(
          "Failed to add event listener for media player: " +
          std::string(libvlc_errmsg()));
    }

    libvlc_media_player_set_media(media_player.get(), media.get());

    // Media is not needed anymore
    media.reset();

    // Turn on fullscreen mode
    libvlc_toggle_fullscreen(media_player.get());

    // Start playing
    if (libvlc_media_player_play(media_player.get()))
    {
      throw std::runtime_error(
          "Failed to start playing: " + std::string(libvlc_errmsg()));
    }

    {
      wait_data_type::unique_lock_type wait_data_lock(wait_data.mutex);
      wait_data.condition.wait(wait_data_lock, [&wait_data]() -> bool
      {
        return wait_data.finished_playing;
      });
    }

    // Stop playing
    libvlc_media_player_stop(media_player.get());

    return EXIT_SUCCESS;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknown error" << std::endl;
  }
}
