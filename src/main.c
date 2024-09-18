/* SPDX-License-Identifier: MIT
 * Copyright 2024      Jorenar
 * Copyright 2022-2024 DavidAlfa
 */

#include <fel.h>
#include <spinand.h>
#include <md5.h>
#include <time.h>


static struct xfel_ctx_t ctx;
static char Name[128];
static size_t capacity;
static uint32_t read_bytes;
static char *flashbf, *filebf;
static char filename[128];           
static char ext[16];
static char * dot;
static time_t start;

static int terminal_error(void){
    if(ctx.hdl){
        libusb_close(ctx.hdl);   
    }
    libusb_exit(NULL);     
    if(flashbf){ free(flashbf); }
    if(filebf) { free(filebf); }
    exit(-1);  
}
static uint32_t file_save(const char * filename, void * buf, uint32_t len){
    FILE * out = fopen(filename, "wb");
    uint32_t r;
    if(!out){ return 0; }
    r = fwrite(buf, len, 1, out);
    fclose(out);
    return r;
}

static void * file_load(const char * filename, uint32_t * len)
{
    uint32_t size, n;
    FILE * in;
    char * buf;
    in = fopen(filename, "rb");
    
    fseek(in, 0, SEEK_END);       // seek to end of file
    size = ftell(in);             // get current file pointer
    fseek(in, 0, SEEK_SET);       // seek back to beginning of file  
    buf = malloc(size);        // allocate size
    if(!buf){
        //printf("Unable to allocate file buffer!\r\n");
        return NULL;
    }
    n = fread(buf, 1, size, in);  // Read whole file  
    if(n < size){                 // Ensure it was completely read
        free(buf);
        return NULL;
    }  
    if(len) { *len = n; }  
    if(in != stdin) { fclose(in); }
    return buf;
}


static void usage(void){
    printf("\r\nHantek DSO2D1x flash utility v0.36\r\n");
    printf("Compiled: %s\r\n", __DATE__);
    printf("Based on xfel v1.2.6 - https://github.com/xboot/xfel\r\n");
    printf("Usage:\r\n");
    printf("    dsoflash -h or --help                         - Show this screen\r\n");
    printf("    dsoflash ver                                  - Get chip version\r\n");
    printf("    dsoflash detect                               - Detect flash\r\n");
    printf("    dsoflash status                               - Get flash status regs\r\n");
    printf("    dsoflash reset                                - Restart device\r\n");
    printf("    dsoflash read <file>                          - Dump flash to file\r\n");
    printf("    dsoflash write <file>                         - Restore flash from file\r\n");
    printf("    dsoflash erase                                - Erase flash\r\n\n");
    printf("Warning: Commands will be executed inmediately, without confirmation!\r\n");
}

static int init_system(void){
    int init=0;
    
    printf("\r\nConfiguring USB to HS mode... ");
    fel_write32(&ctx, 0x01c13040, 0x29860);
    libusb_close(ctx.hdl);                                                  // Close USB
    
    for(int i=0; i<10; i++){                                                 // Try for 10 seconds
        sleep(1);                                                           // Wait 1 seconds for USB reenumeration    
        ctx.hdl = libusb_open_device_with_vid_pid(NULL, 0x1f3a, 0xefe8);    // Open USB device
        if(ctx.hdl){                                                        // If sucessfull
            init = fel_init(&ctx);                                          // Try initialization
            if(init){ break; }                                              // Break on success
            libusb_close(ctx.hdl);                                          // Otherwise close handler and retry
        }
    }
    
    if(!init){
        printf("ERROR: No FEL device found\r\n");
        return -1;
    }
    else{
         printf("OK\r\n");
    }
    
    if(!spinand_detect(&ctx, Name, &capacity)){
        terminal_error();
        printf("Unknown flash memory!\r\n");
        return 1;
    }
    printf("Flash found: '%s'  Size: %zu MB\r\n\n", Name, capacity/((size_t)1024*1024));
    return 0;
}

void compute_md5(char * data, uint32_t len, char * digest){
    struct UL_MD5Context md5_ctx;    
    unsigned char d[UL_MD5LENGTH];
    
    ul_MD5Init(&md5_ctx);
    ul_MD5Update(&md5_ctx, (uint8_t*)data, len);
    ul_MD5Final(d, &md5_ctx);
    sprintf(digest, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7],d[8],d[9],d[10],d[11],d[12],d[13],d[14],d[15]);
            
    digest[32] = 0;
}
    
void process_filename(char * s){
    strcpy(filename, s);
    dot = strrchr(filename, '.');
    if(!dot){
        dot = &filename[strlen(filename)];
        strcpy(dot, ".bin");
    }
    strcpy(ext, dot);
}    
void show_elapsed(void){
    char time_str[100];
    time_t elapsed = time(0) - start;
    strftime(time_str, sizeof(time_str) - 1, "Elapsed time: %M:%S\r\n", gmtime(&elapsed));
    printf("%s\n", time_str);
}

int main(int argc, char * argv[]){    
    if(argc < 2)
    {
        usage();
        return 0;
    }  
    argc --;
    argv ++;
    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            usage();
            return 0;
        }
    }
    libusb_init(NULL);  
    ctx.hdl = libusb_open_device_with_vid_pid(NULL, 0x1f3a, 0xefe8);
    if(ctx.hdl == NULL){        
        printf("ERROR: No USB device found\r\n");
        libusb_exit(NULL);
        return -1;
    }
    if(!fel_init(&ctx)){
        printf("ERROR: No FEL device found\r\n");
        libusb_exit(NULL);
        return -1;
    }   
    if(!strcmp(argv[0], "ver")){
        printf("%.8s ID=0x%08x(%s) dflag=0x%02x dlength=0x%02x scratchpad=0x%08x\r\n",
            ctx.version.magic, ctx.version.id, ctx.chip->name, ctx.version.dflag,
            ctx.version.dlength, ctx.version.scratchpad);
    }
    else if(!strcmp(argv[0], "detect") && (argc == 1)){
        init_system();
    }  
    else if(!strcmp(argv[0], "status") && (argc == 1)){
        dso2d_dump_regs(&ctx);
    }    
	else if(!strcmp(argv[0], "reset")){
		fel_chip_reset(&ctx);
	}
    else if(!strcmp(argv[0], "erase") && (argc == 1)){
        dso2d_erase(&ctx);
    }
    else if(!strcmp(argv[0], "read") && (argc == 2)){
        init_system();
        process_filename(argv[1]);
        flashbf = malloc(capacity);
        if(!flashbf){
            printf("Unable to allocate flash buffer!\r\n");
            terminal_error();        
        }        
        start = time(0);
        dso2d_dump(&ctx, flashbf);
        if(!file_save(filename, flashbf, capacity)){
            printf("Unable to write to file %s!\r\n", filename);
            terminal_error();              
        }
        else{
            char data_md5[33];   
            printf("\nFlash saved to %s\r\n", filename);
            strcpy(dot, ".md5");
            compute_md5(flashbf, capacity, data_md5);            
            if(!file_save(filename, data_md5, sizeof(data_md5))){
                printf("Unable to write file %s!\r\n\nMD5: %s\r\n", filename, data_md5);
            }
            else{
                printf("%s\r\n\nMD5: %s\r\n", filename, data_md5);
            }
            show_elapsed();
            free(flashbf);            
        }
    }
    else if(!strcmp(argv[0], "write") && (argc == 2)){       
        init_system();
        
        char data_md5[33];          
        process_filename(argv[1]);        
        strcpy(dot, ".md5");        
        char * file_md5 = file_load(filename, &read_bytes);
        if(file_md5 != NULL && read_bytes != 33){
            printf("Bad MD5 filesize, must be 33 Bytes!\r\n");
            terminal_error();      
        }
        
        filebf = file_load(argv[1], &read_bytes);                 
        if(!filebf){
            printf("Unable to read from file %s!\r\n", argv[1]);
            terminal_error();      
        }
        
        compute_md5(filebf, capacity, data_md5);   
        
        if (read_bytes != capacity){                           // capacity not matching flash size
            size_t spare;                             // Check  if filesize matches data+spare
            if(read_bytes==((size_t)132*1024*1024)){                 // 64 byte spare area
                spare=64;
            }
            else if(read_bytes==((size_t)136*1024*1024)){            // 128 byte spare area
                spare=128;
            }
            else if(read_bytes==((size_t)144*1024*1024)){            // 256 byte spare area
                spare=256;
            }
            else{
                printf("File doesn't match the flash size\r\n");
                printf(" Flash: %zu Bytes,   File: %zu Bytes\r\n", read_bytes, capacity);
                terminal_error();
            }
            
            printf("Old backup detected, spare area: %zuBytes\r\n\n", spare);
            
            char * tmp = malloc(read_bytes);                      // Temporal buffer
            if(tmp==NULL){                
                printf("Unable to allocate flash buffer!\r\n");
                terminal_error();      
            }
            char *in=filebf, *out=tmp;
            for(size_t i=0; i<read_bytes; i+=(2048+spare)){         // Extract data
                memcpy(out, in, 2048);
                in+=2048+spare;
                out+=2048;
            }
            memcpy(filebf,tmp,capacity);                        // Copy data
            free(tmp);
        }
        
        if(!file_md5){
            printf("MD5: %s\r\nFile %s not found, skipping md5 check\r\n", data_md5, filename);
        }
        else if(strcmp(data_md5, file_md5) != 0){
            printf("MD5 mismatch! Aborting...\r\n\n%s: %s\r\nComputed: %s\r\n\n", filename, file_md5, data_md5);
            printf("You might delete or rename the md5 file to skip md5 check\r\n");
            terminal_error();
        }
        else{
            printf("MD5 OK: %s\r\n", data_md5);
        }
        if(file_md5){
            free(file_md5);
        }
          
        start = time(0);     
        dso2d_restore(&ctx, filebf);        
        printf("\nFlash written sucessfully from file %s\r\n", argv[1]);
        show_elapsed();
        free(filebf);
    }
    else{
        usage();
    }
  
    libusb_close(ctx.hdl);
    libusb_exit(NULL);
    return 0;
}
