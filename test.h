

bool sd_init();
void writeFile(const char *path, uint8_t *data, size_t len);
void photo_save(const char *fileName, uint8_t *buf, int len);
uint8_t *loadJpeg();