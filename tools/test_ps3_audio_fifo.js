// Emit a host test harness around the actual write function, not a copy.
// node tools/test_ps3_audio_fifo.js | docker run --rm -i ps3dev-gcc13:rsxfix \
//   sh -c 'cc -x c -o /tmp/audio-test - && /tmp/audio-test'
const fs = require('fs');
const path = require('path');
const source = fs.readFileSync(path.join(__dirname, '../audio/drivers/ps3_audio.c'), 'utf8');
const start = source.indexOf('static ssize_t ps3_audio_write(');
const end = source.indexOf('static bool ps3_audio_stop(', start);
if (start < 0 || end < 0) throw new Error('Cannot locate PS3 write function');
console.log(`#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#define AUDIO_CHANNELS 2
#define PS3_SYS_NO_TIMEOUT 0
#define CELL_OK 0
typedef struct { size_t used; } fifo_t;
typedef struct { fifo_t *buffer; int lock, cond; bool nonblock, quit_thread; } ps3_audio_t;
static unsigned waits;
static int wait_error;
static ps3_audio_t *active;
static unsigned char captured[256];
static size_t captured_size;
static int sysLwMutexLock(int *lock, int timeout) { assert(!*lock); *lock=1; return 0; }
static int sysLwMutexUnlock(int *lock) { assert(*lock); *lock=0; return 0; }
#define FIFO_WRITE_AVAIL(f) (32-(f)->used)
static void fifo_write(fifo_t *f, const void *buf, size_t n) {
 assert(active->lock); assert(n<=FIFO_WRITE_AVAIL(f));
 assert(captured_size+n<=sizeof(captured));
 memcpy(captured+captured_size,buf,n); captured_size+=n; f->used+=n;
}
static int sysLwCondWait(int *cond, int timeout) {
 assert(active->lock); ++waits;
 if(wait_error) return -1;
 active->buffer->used=0; return 0;
}
`);
console.log(source.slice(start, end));
console.log(`int main(void) {
 fifo_t fifo={0}; ps3_audio_t aud={&fifo,0,0,false,false};
 unsigned char input[96]; unsigned i;
 for(i=0;i<sizeof(input);++i) input[i]=(unsigned char)i;
 active=&aud;
 assert(ps3_audio_write(&aud,input,sizeof(input))==96);
 assert(waits==2 && !aud.lock && captured_size==96);
 assert(!memcmp(input,captured,96));
 aud.nonblock=true; captured_size=0; waits=0;
 assert(ps3_audio_write(&aud,input,16)==0);
 assert(waits==0 && !aud.lock);
 fifo.used=16;
 assert(ps3_audio_write(&aud,input,32)==16);
 assert(captured_size==16 && !memcmp(input,captured,16) && !aud.lock);
 aud.nonblock=false; wait_error=1;
 assert(ps3_audio_write(&aud,input,16)==-1 && !aud.lock);
 fifo.used=16;
 assert(ps3_audio_write(&aud,input,32)==16 && !aud.lock);
 aud.quit_thread=true;
 assert(ps3_audio_write(&aud,input,8)==0 && !aud.lock);
 puts("PASS: chunked blocking write, nonblocking full/partial, wait error, shutdown, lock ownership");
 return 0;
}`);
