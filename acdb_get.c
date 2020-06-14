#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "acdb_data.h"

#define LIB_AUDIO_HAL "audio.primary." BOARD_PLATFORM ".so"

typedef int (*platform_get_snd_device_acdb_id_t)(int);
typedef int (*platform_get_snd_device_index_t)(char *);
typedef const char *(*platform_get_snd_device_name_t)(int);

int main(int argv, const char **argc) {
    platform_get_snd_device_acdb_id_t platform_get_snd_device_acdb_id = NULL;
    platform_get_snd_device_index_t platform_get_snd_device_index = NULL;
    platform_get_snd_device_name_t platform_get_snd_device_name = NULL;
    void *handle = NULL;
    int i;

    if (argv < 2)
        fprintf(stderr, "Usage: %s <path to HAL>\n", argc[0]);

    handle = dlopen(argc[1], RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }
    fprintf(stderr, "Loaded %s\n\n", argc[1]);

    // /* Try ld path first */
    // handle = dlopen(LIB_AUDIO_HAL, RTLD_LAZY);
    // if (!handle) {
    //     char audio_hal[1024] = {0};

    //     snprintf(audio_hal, sizeof(audio_hal),
    //              "/vendor/lib/hw/%s", LIB_AUDIO_HAL);

    // } else {
    //     fprintf(stderr, "Loaded %s\n\n", LIB_AUDIO_HAL);
    // }

    platform_get_snd_device_acdb_id = (platform_get_snd_device_acdb_id_t) dlsym(handle, "platform_get_snd_device_acdb_id");
    if (platform_get_snd_device_acdb_id == NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    platform_get_snd_device_name = (platform_get_snd_device_name_t) dlsym(handle, "platform_get_snd_device_name");
    if (platform_get_snd_device_name == NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    platform_get_snd_device_index =
        (platform_get_snd_device_index_t)dlsym(handle, "platform_get_snd_device_index");
    if (!platform_get_snd_device_index)  {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    printf("    <acdb_ids>\n");
    for (i = 1; i < SND_DEVICE_MAX; i++) {
        int dev = platform_get_snd_device_index(device_table[i]);
        const char *device = platform_get_snd_device_name(dev);
        int acdb_id = platform_get_snd_device_acdb_id(dev);

        if (acdb_id <= 0) {
            continue;
        }

        printf("        <!--%s-->\n", device);
        printf("        <device name=\"%s\" acdb_id=\"%d\"/>\n",
               device_table[i], acdb_id);
    }
    printf("    </acdb_ids>\n");

    dlclose(handle);
    return 0;
}
