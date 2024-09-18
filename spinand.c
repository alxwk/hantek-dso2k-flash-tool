#include <spinand.h>

#define SPINAND_ID(...)    { .val = { __VA_ARGS__ }, .len = sizeof((uint8_t[]){ __VA_ARGS__ }) }


struct spinand_info_t {
    char * name;
    struct {
        uint8_t val[4];
        uint8_t len;
    } id;
    uint32_t page_size;
    uint32_t spare_size;
    uint32_t pages_per_block;
    uint32_t blocks_per_die;
    uint32_t planes_per_die;
    uint32_t ndies;
};

struct spinand_pdata_t {
    struct spinand_info_t info;
    uint32_t swapbuf;
    uint32_t swaplen;
    uint32_t cmdlen;
};

enum {
    OPCODE_RDID                    = 0x9f,
    OPCODE_GET_FEATURE            = 0x0f,
    OPCODE_SET_FEATURE            = 0x1f,
    OPCODE_FEATURE_PROTECT        = 0xa0,
    OPCODE_FEATURE_CONFIG        = 0xb0,
    OPCODE_FEATURE_STATUS        = 0xc0,
    OPCODE_READ_PAGE_TO_CACHE    = 0x13,
    OPCODE_FAST_READ            = 0x0B,
    OPCODE_READ_PAGE_FROM_CACHE    = 0x03,
    OPCODE_WRITE_ENABLE            = 0x06,
    OPCODE_BLOCK_ERASE            = 0xd8,
    OPCODE_PROGRAM_LOAD            = 0x02,
    OPCODE_PROGRAM_EXEC            = 0x10,
    OPCODE_RESET                = 0xff,
};

static const struct spinand_info_t spinand_infos[] = {
	/* Winbond */
	{ "W25N512GV",       SPINAND_ID(0xef, 0xaa, 0x20), 2048,  64,  64,  512, 1, 1 },
	{ "W25N01GV",        SPINAND_ID(0xef, 0xaa, 0x21), 2048,  64,  64, 1024, 1, 1 },
	{ "W25M02GV",        SPINAND_ID(0xef, 0xab, 0x21), 2048,  64,  64, 1024, 1, 2 },
	{ "W25N02KV",        SPINAND_ID(0xef, 0xaa, 0x22), 2048, 128,  64, 2048, 1, 1 },

	/* Gigadevice */
	{ "GD5F1GQ4UAWxx",   SPINAND_ID(0xc8, 0x10),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ5UExxG",   SPINAND_ID(0xc8, 0x51),       2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExIG",   SPINAND_ID(0xc8, 0xd1),       2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExxH",   SPINAND_ID(0xc8, 0xd9),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ4xAYIG",   SPINAND_ID(0xc8, 0xf1),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F2GQ4UExIG",   SPINAND_ID(0xc8, 0xd2),       2048, 128,  64, 2048, 1, 1 },
	{ "GD5F2GQ5UExxH",   SPINAND_ID(0xc8, 0x32),       2048,  64,  64, 2048, 1, 1 },
	{ "GD5F2GQ4xAYIG",   SPINAND_ID(0xc8, 0xf2),       2048,  64,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UBxIG",   SPINAND_ID(0xc8, 0xd4),       4096, 256,  64, 2048, 1, 1 },
	{ "GD5F4GQ4xAYIG",   SPINAND_ID(0xc8, 0xf4),       2048,  64,  64, 4096, 1, 1 },
	{ "GD5F2GQ5UExxG",   SPINAND_ID(0xc8, 0x52),       2048, 128,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UCxIG",   SPINAND_ID(0xc8, 0xb4),       4096, 256,  64, 2048, 1, 1 },
	{ "GD5F4GQ4RCxIG",   SPINAND_ID(0xc8, 0xa4),       4096, 256,  64, 2048, 1, 1 },

	/* Macronix */
	{ "MX35LF1GE4AB",    SPINAND_ID(0xc2, 0x12),       2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF1G24AD",    SPINAND_ID(0xc2, 0x14),       2048, 128,  64, 1024, 1, 1 },
	{ "MX31LF1GE4BC",    SPINAND_ID(0xc2, 0x1e),       2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF2GE4AB",    SPINAND_ID(0xc2, 0x22),       2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF2G24AD",    SPINAND_ID(0xc2, 0x24),       2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2GE4AD",    SPINAND_ID(0xc2, 0x26),       2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2G14AC",    SPINAND_ID(0xc2, 0x20),       2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF4G24AD",    SPINAND_ID(0xc2, 0x35),       4096, 256,  64, 2048, 1, 1 },
	{ "MX35LF4GE4AD",    SPINAND_ID(0xc2, 0x37),       4096, 256,  64, 2048, 1, 1 },

	/* Micron */
	{ "MT29F1G01AAADD",  SPINAND_ID(0x2c, 0x12),       2048,  64,  64, 1024, 1, 1 },
	{ "MT29F1G01ABAFD",  SPINAND_ID(0x2c, 0x14),       2048, 128,  64, 1024, 1, 1 },
	{ "MT29F2G01AAAED",  SPINAND_ID(0x2c, 0x9f),       2048,  64,  64, 2048, 2, 1 },
	{ "MT29F2G01ABAGD",  SPINAND_ID(0x2c, 0x24),       2048, 128,  64, 2048, 2, 1 },
	{ "MT29F4G01AAADD",  SPINAND_ID(0x2c, 0x32),       2048,  64,  64, 4096, 2, 1 },
	{ "MT29F4G01ABAFD",  SPINAND_ID(0x2c, 0x34),       4096, 256,  64, 2048, 1, 1 },
	{ "MT29F4G01ADAGD",  SPINAND_ID(0x2c, 0x36),       2048, 128,  64, 2048, 2, 2 },
	{ "MT29F8G01ADAFD",  SPINAND_ID(0x2c, 0x46),       4096, 256,  64, 2048, 1, 2 },

	/* Toshiba */
	{ "TC58CVG0S3HRAIG", SPINAND_ID(0x98, 0xc2),       2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIG", SPINAND_ID(0x98, 0xcb),       2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIG", SPINAND_ID(0x98, 0xcd),       4096, 256,  64, 2048, 1, 1 },
	{ "TC58CVG0S3HRAIJ", SPINAND_ID(0x98, 0xe2),       2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIJ", SPINAND_ID(0x98, 0xeb),       2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIJ", SPINAND_ID(0x98, 0xed),       4096, 256,  64, 2048, 1, 1 },
	{ "TH58CVG3S0HRAIJ", SPINAND_ID(0x98, 0xe4),       4096, 256,  64, 4096, 1, 1 },

	/* Esmt */
	{ "F50L512M41A",     SPINAND_ID(0xc8, 0x20),       2048,  64,  64,  512, 1, 1 },
	{ "F50L1G41A",       SPINAND_ID(0xc8, 0x21),       2048,  64,  64, 1024, 1, 1 },
	{ "F50L1G41LB",      SPINAND_ID(0xc8, 0x01),       2048,  64,  64, 1024, 1, 1 },
	{ "F50L2G41LB",      SPINAND_ID(0xc8, 0x0a),       2048,  64,  64, 1024, 1, 2 },

	/* Fison */
	{ "CS11G0T0A0AA",    SPINAND_ID(0x6b, 0x00),       2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0G0A0AA",    SPINAND_ID(0x6b, 0x10),       2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0S0A0AA",    SPINAND_ID(0x6b, 0x20),       2048,  64,  64, 1024, 1, 1 },
	{ "CS11G1T0A0AA",    SPINAND_ID(0x6b, 0x01),       2048, 128,  64, 2048, 1, 1 },
	{ "CS11G1S0A0AA",    SPINAND_ID(0x6b, 0x21),       2048,  64,  64, 2048, 1, 1 },
	{ "CS11G2T0A0AA",    SPINAND_ID(0x6b, 0x02),       2048, 128,  64, 4096, 1, 1 },
	{ "CS11G2S0A0AA",    SPINAND_ID(0x6b, 0x22),       2048,  64,  64, 4096, 1, 1 },

	/* Etron */
	{ "EM73B044VCA",     SPINAND_ID(0xd5, 0x01),       2048,  64,  64,  512, 1, 1 },
	{ "EM73C044SNB",     SPINAND_ID(0xd5, 0x11),       2048, 120,  64, 1024, 1, 1 },
	{ "EM73C044SNF",     SPINAND_ID(0xd5, 0x09),       2048, 128,  64, 1024, 1, 1 },
	{ "EM73C044VCA",     SPINAND_ID(0xd5, 0x18),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNA",     SPINAND_ID(0xd5, 0x19),       2048,  64, 128,  512, 1, 1 },
	{ "EM73C044VCD",     SPINAND_ID(0xd5, 0x1c),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SND",     SPINAND_ID(0xd5, 0x1d),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73D044SND",     SPINAND_ID(0xd5, 0x1e),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73C044VCC",     SPINAND_ID(0xd5, 0x22),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044VCF",     SPINAND_ID(0xd5, 0x25),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNC",     SPINAND_ID(0xd5, 0x31),       2048, 128,  64, 1024, 1, 1 },
	{ "EM73D044SNC",     SPINAND_ID(0xd5, 0x0a),       2048, 120,  64, 2048, 1, 1 },
	{ "EM73D044SNA",     SPINAND_ID(0xd5, 0x12),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNF",     SPINAND_ID(0xd5, 0x10),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCA",     SPINAND_ID(0xd5, 0x13),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCB",     SPINAND_ID(0xd5, 0x14),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCD",     SPINAND_ID(0xd5, 0x17),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCH",     SPINAND_ID(0xd5, 0x1b),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044SND",     SPINAND_ID(0xd5, 0x1d),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCG",     SPINAND_ID(0xd5, 0x1f),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCE",     SPINAND_ID(0xd5, 0x20),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCL",     SPINAND_ID(0xd5, 0x2e),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNB",     SPINAND_ID(0xd5, 0x32),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73E044SNA",     SPINAND_ID(0xd5, 0x03),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044SND",     SPINAND_ID(0xd5, 0x0b),       4096, 240,  64, 2048, 1, 1 },
	{ "EM73E044SNB",     SPINAND_ID(0xd5, 0x23),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCA",     SPINAND_ID(0xd5, 0x2c),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCB",     SPINAND_ID(0xd5, 0x2f),       2048, 128,  64, 4096, 1, 1 },
	{ "EM73F044SNA",     SPINAND_ID(0xd5, 0x24),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73F044VCA",     SPINAND_ID(0xd5, 0x2d),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73E044SNE",     SPINAND_ID(0xd5, 0x0e),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73C044SNG",     SPINAND_ID(0xd5, 0x0c),       2048, 120,  64, 1024, 1, 1 },
	{ "EM73D044VCN",     SPINAND_ID(0xd5, 0x0f),       2048,  64,  64, 2048, 1, 1 },

	/* Elnec */
	{ "FM35Q1GA",        SPINAND_ID(0xe5, 0x71),       2048,  64,  64, 1024, 1, 1 },

	/* Paragon */
	{ "PN26G01A",        SPINAND_ID(0xa1, 0xe1),       2048, 128,  64, 1024, 1, 1 },
	{ "PN26G02A",        SPINAND_ID(0xa1, 0xe2),       2048, 128,  64, 2048, 1, 1 },

	/* Ato */
	{ "ATO25D1GA",       SPINAND_ID(0x9b, 0x12),       2048,  64,  64, 1024, 1, 1 },

	/* Heyang */
	{ "HYF1GQ4U",        SPINAND_ID(0xc9, 0x51),       2048, 128,  64, 1024, 1, 1 },
	{ "HYF2GQ4U",        SPINAND_ID(0xc9, 0x52),       2048, 128,  64, 2048, 1, 1 },
	{ "HYF4GQ4U",        SPINAND_ID(0xc9, 0x54),       2048, 128,  64, 4096, 1, 1 },

	/* FORESEE */
	{ "F35SQA001G",      SPINAND_ID(0xCD, 0x71, 0x71), 2048,  64,  64, 1024, 1, 1 },
	{ "F35SQA002G",      SPINAND_ID(0xCD, 0x72, 0x72), 2048,  64,  64, 2048, 1, 1 },
};


static inline int spinand_info(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
    const struct spinand_info_t * t;
    uint8_t tx[2];
    uint8_t rx[4];
    int i;

    tx[0] = OPCODE_RDID;
    tx[1] = 0x0;
    if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 2, rx, 4))
    {
        return 0;
    }
    for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
    {
        t = &spinand_infos[i];
        if(memcmp(rx, t->id.val, t->id.len) == 0)
        {
            memcpy(&pdat->info, t, sizeof(struct spinand_info_t));
            return 1;
        }
    }
    tx[0] = OPCODE_RDID;
    if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 1, rx, 4))
        return 0;
    for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
    {
        t = &spinand_infos[i];
        if(memcmp(rx, t->id.val, t->id.len) == 0)
        {
            memcpy(&pdat->info, t, sizeof(struct spinand_info_t));
            return 1;
        }
    }
    printf("The spi nand flash '0x%02x%02x%02x%02x' is not yet supported\r\n", rx[0], rx[1], rx[2], rx[3]);
    return 0;
}

static inline int spinand_reset(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
    uint8_t tx[1] = { OPCODE_RESET };
    if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, sizeof(tx), 0, 0))
    {
        return 0;
    }
    usleep(100U * 1000);
    return 1;
}

static inline int spinand_get_feature(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint8_t addr, uint8_t * val)
{
    uint8_t tx[2] = {OPCODE_GET_FEATURE, addr };
    if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, sizeof(tx), val, 1))
    {
        return 0;
    }
    return 1;
}

static inline int spinand_set_feature(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint8_t addr, uint8_t val)
{
    uint8_t tx[3];

    tx[0] = OPCODE_SET_FEATURE;
    tx[1] = addr;
    tx[2] = val;
    if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 3, 0, 0))
    {
        return 0;
    }
    return 1;
}

static inline int spinand_wait_for_busy(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
    uint8_t cbuf[256];
    uint32_t clen = 0;

    cbuf[clen++] = SPI_CMD_SELECT;
    cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
    cbuf[clen++] = SPI_CMD_DESELECT;
    cbuf[clen++] = SPI_CMD_END;
    if(clen <= pdat->cmdlen)
    {
        fel_chip_spi_run(ctx, cbuf, clen);
        return 1;
    }
    return 0;
}

static int spinand_helper_init(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, int unlock)
{
    uint8_t val;
    if(fel_spi_init(ctx, &pdat->swapbuf, &pdat->swaplen, &pdat->cmdlen) && spinand_info(ctx, pdat))
    {
        spinand_reset(ctx, pdat);
        spinand_wait_for_busy(ctx, pdat);
        if(unlock)
        {
          if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, &val) ){              // Read Status-1 register
            if(val != 0)
            {
              spinand_wait_for_busy(ctx, pdat);
              if(!spinand_set_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, 0)){
                printf("Error while modifying Status-1 register!r\n");
                return 0;
              }
              else
              {
                spinand_wait_for_busy(ctx, pdat);
                if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, &val) )
                {            
                  if(val != 0)
                  {
                    printf("Unable to modify disable Status-1 register!\r\n");
                    return 0;
                  }
                }
                else
                {
                  printf("Error reading Status-1 register!\r\n");
                  return 0;
                }
              }
            }
          }
          else
          {
            printf("Error reading Status-1 register!\r\n");
            return 0;
          }
        }
        spinand_wait_for_busy(ctx, pdat);
        if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_CONFIG, &val))             // Read Status-2 register
        {                
            if( (val & 0x10) != 0x10 )                                              // Check ECC-E=1
            {
                val |= 0x10;
                spinand_wait_for_busy(ctx, pdat);
                if(!spinand_set_feature(ctx, pdat, OPCODE_FEATURE_CONFIG, val))     // Enable ECC
                {
                printf("Error while modifying Status-2 register!\r\n");
                return 0;
                }
                else
                {          
                    spinand_wait_for_busy(ctx, pdat);
                    if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_CONFIG, &val))
                    {            
                        if( (val & 0x10) != 0x10 )
                        {
                            printf("Unable modify Status-2 register!\r\n");
                            return 0;
                        }
                    }
                    else
                    {
                        printf("Error reading Status-2 register!\r\n");
                        return 0;
                    }
                }
            }
        }
        else
        {
            printf("Error reading Status-2 register!\r\n");
            return 0;
        }    
        spinand_wait_for_busy(ctx, pdat);
        return 1;
    }
    return 0;
}

int spinand_detect(struct xfel_ctx_t * ctx, char * name, size_t * capacity)
{
    struct spinand_pdata_t pdat;
    if(spinand_helper_init(ctx, &pdat, 0))
    {
        if(name)
        {
            strcpy(name, pdat.info.name);
        }
        if(capacity)
        {
            *capacity = (size_t)pdat.info.page_size * pdat.info.pages_per_block * pdat.info.blocks_per_die * pdat.info.ndies;
        }
        return 1;
    }
    return 0;
}
#define ERASE_CMD_SZ    (64U)
int dso2d_erase(struct xfel_ctx_t * ctx){
    struct progress_t p;
    struct spinand_pdata_t pdat;    
    uint8_t cbuf[(ERASE_CMD_SZ*16)+1];
    for(uint32_t i=0; i<ERASE_CMD_SZ; i++)                                // Make a large cmd queue to reduce overhead
    {
        uint8_t * d = &cbuf[16*i];
        d[0]  = SPI_CMD_SELECT;                 // Write enable
        d[1]  = SPI_CMD_FAST;
        d[2]  = 1;
        d[3]  = OPCODE_WRITE_ENABLE;
        d[4]  = SPI_CMD_DESELECT;
        d[5]  = SPI_CMD_SELECT;
        d[6]  = SPI_CMD_FAST;
        d[7]  = 4;
        d[8]  = OPCODE_BLOCK_ERASE;             // Erase block
        d[9]  = 0;                              // Dummy
        d[10] = 0;                              // Block address
        d[11] = 0;
        d[12] = SPI_CMD_DESELECT;
        d[13] = SPI_CMD_SELECT;                 // Check busy
        d[14] = SPI_CMD_SPINAND_WAIT;
        d[15] = SPI_CMD_DESELECT;
    };
    cbuf[16*ERASE_CMD_SZ] = SPI_CMD_END;        // Done
    
    if(!spinand_helper_init(ctx, &pdat, 1) || sizeof(cbuf) > pdat.cmdlen )
    {
        return 0;
    }
    
    uint32_t pages, page=0, n = pdat.info.page_size;
    
    printf("\r\nErasing flash...\r\n");
    pages = pdat.info.pages_per_block*pdat.info.blocks_per_die*pdat.info.ndies*pdat.info.planes_per_die;
    progress_start(&p, pages*n);
    while(page < pages)
    {
        for(uint32_t i=0; i<ERASE_CMD_SZ; i++)                                // Make a large cmd queue to reduce overhead
        {
            uint8_t * d = &cbuf[16*i];
            d[10] = (page>>8) & 0xFF;                              // Block address
            d[11] = (page>>0) & 0xFF;
            page += pdat.info.pages_per_block;
        };
        fel_chip_spi_run(ctx, cbuf, sizeof(cbuf));              // Run Command buffer
        progress_update(&p, ERASE_CMD_SZ*n*pdat.info.pages_per_block);
    }
    progress_stop(&p);
    return 1;
}

#define RX_CMD_SZ       (28U)
#define RX_BLOCK_SIZE   (128U)
int dso2d_dump(struct xfel_ctx_t * ctx, void *buf)
{
    struct spinand_pdata_t pdat;
    
    if(!spinand_helper_init(ctx, &pdat, 0))
    {
        return 0;    
    }
        
    struct progress_t p;
    uint32_t block_size = RX_BLOCK_SIZE;                                // Read blocks of n pages
    uint32_t page=0, pages = pdat.info.pages_per_block*pdat.info.blocks_per_die*pdat.info.ndies*pdat.info.planes_per_die;
    uint32_t page_size = pdat.info.page_size;
    uint32_t read_size = block_size * page_size;
    uint8_t cbuf[(RX_CMD_SZ*block_size) + 1];    

    for(uint32_t i=0; i<block_size; i++)                                // Make a large cmd queue to reduce overhead
    {
        uint8_t * d = &cbuf[RX_CMD_SZ*i];
        
        d[0] = SPI_CMD_SELECT;
        d[1] = SPI_CMD_FAST;
        d[2] = 4;
        d[3] = OPCODE_READ_PAGE_TO_CACHE;                               // Load page into buffer
        d[4] = 0;                                                       // Dummy
                                                                        // 5-6 Page address to read, updated later
        d[7] = SPI_CMD_DESELECT;
        d[8] = SPI_CMD_SELECT;
        d[9] = SPI_CMD_SPINAND_WAIT;                                    // Check Busy flag
        d[10] = SPI_CMD_DESELECT;      
        d[11] = SPI_CMD_SELECT;
        d[12] = SPI_CMD_FAST;
        d[13] = 4;
        d[14] = OPCODE_READ_PAGE_FROM_CACHE;                            // Read data from buffer
        d[15] = 0;                                                      // Column address H
        d[16] = 0;                                                      // Column address L
        d[17] = 0;                                                      // Dummy
        d[18] = SPI_CMD_RXBUF;                                          // Receive data into RX Buffer
                                                                        // 19-22 Dest address,  updated later
        d[23] = (page_size>>0)  & 0xFF;                                 // Rx length = page size + spare size
        d[24] = (page_size>>8)  & 0xFF;
        d[25] = (page_size>>16) & 0xFF;
        d[26] = (page_size>>24) & 0xFF;
        d[27] = SPI_CMD_DESELECT;
    };
    
    cbuf[RX_CMD_SZ*block_size] = SPI_CMD_END;
       
    if(sizeof(cbuf) > pdat.cmdlen )
    {        
      printf("cbuf: is too large for cmdbuf! %u : %u\r\n", sizeof(cbuf), pdat.cmdlen);
      return 0;    
    }
    
    printf("Reading flash...\r\n");    
    progress_start(&p, pages*page_size);
    
    while(page < pages)    
    {
        for(uint32_t i=0; i<block_size; i++)
        {
            uint8_t * d = &cbuf[RX_CMD_SZ*i];
            uint32_t p = page+i;
            uint32_t dst_addr = pdat.swapbuf+(i*page_size);

            d[5] = (p>>8)  & 0xFF;                                      // Page address to read H
            d[6] = (p>>0)  & 0xFF;                                      // Page address to read L
            d[19] = (dst_addr>>0)  & 0xFF;                              // Dest address
            d[20] = (dst_addr>>8)  & 0xFF;
            d[21] = (dst_addr>>16) & 0xFF;
            d[22] = (dst_addr>>24) & 0xFF;
        }
        fel_chip_spi_run(ctx, cbuf, sizeof(cbuf));                      // Run Command buffer    
        fel_read(ctx, pdat.swapbuf, buf, read_size);                    // Receive RX buffer
        buf += read_size;                                               
        page += block_size;
        progress_update(&p, read_size);
    }
    progress_stop(&p);
    return 1;
}


#define TX_CMD_SZ       (32U)
#define TX_BLOCK_SIZE   (128U)
int dso2d_restore(struct xfel_ctx_t * ctx, void * buf)
{
    struct spinand_pdata_t pdat;
    
    if(!dso2d_erase(ctx) || !spinand_helper_init(ctx, &pdat, 1)){
        return 0;    
    }

    struct progress_t p;
    uint32_t block_size = TX_BLOCK_SIZE;                                // Write blocks of n pages
    uint32_t page=0, pages = pdat.info.pages_per_block*pdat.info.blocks_per_die*pdat.info.ndies*pdat.info.planes_per_die;
    uint32_t page_size = pdat.info.page_size;
    uint32_t pages_to_write = 0;
    uint8_t cbuf[(TX_CMD_SZ*block_size) + 1];                           // Make a large cmd queue to reduce overhead
    uint8_t dbuf[block_size*page_size];
        
    if(sizeof(cbuf) > pdat.cmdlen )
    {        
        printf("cbuf is too large for cmdbuf! %u : %u\r\n", sizeof(cbuf), pdat.cmdlen);
        return 0;    
    }
    
    printf("\r\nWriting flash...\r\n");
    progress_start(&p, pages*page_size);
    uint32_t last_page=0, i;
    uint8_t * d = buf;
    while(page < pages)    
    {   
        i=0;        
        pages_to_write=0;
        
        for(uint32_t j=0; (j<page_size) && (page<pages); ){                 // Scan data for empty pages (All FF), fill data buffer
            if(d[j] != 0xFF){                                               // If not FF data found, store page
                memcpy(&dbuf[i*page_size], d, page_size);                   // Copy page data
                uint8_t * c = &cbuf[i*TX_CMD_SZ];
                
                c[0]  = SPI_CMD_SELECT;                                     // Fill cmd data
                c[1]  = SPI_CMD_FAST;
                c[2]  = 1;
                c[3]  = OPCODE_WRITE_ENABLE;                                // Write enable cmd
                c[4]  = SPI_CMD_DESELECT;
                c[5]  = SPI_CMD_SELECT;
                c[6]  = SPI_CMD_FAST;
                c[7]  = 3;
                c[8]  = OPCODE_PROGRAM_LOAD;                                // Program load cmd (Write to flash buffer)
                c[9]  = 0;                                                  // Column address H
                c[10] = 0;                                                  // Column address L
                c[11] = SPI_CMD_TXBUF;                                      // Transfer contents from TX Buffer
                c[12] = ((pdat.swapbuf+(i*page_size))>>0)  & 0xFF;          // Src address = SDRAM
                c[13] = ((pdat.swapbuf+(i*page_size))>>8)  & 0xFF;
                c[14] = ((pdat.swapbuf+(i*page_size))>>16) & 0xFF;
                c[15] = ((pdat.swapbuf+(i*page_size))>>24) & 0xFF;
                c[16] = (page_size>>0)  & 0xFF;                             // Tx length = page size + spare size
                c[17] = (page_size>>8)  & 0xFF;
                c[18] = (page_size>>16) & 0xFF;
                c[19] = (page_size>>24) & 0xFF;
                c[20] = SPI_CMD_DESELECT;
                c[21] = SPI_CMD_SELECT;
                c[22] = SPI_CMD_FAST;
                c[23] = 4;
                c[24] = OPCODE_PROGRAM_EXEC;                                // Execute program (Write page)
                c[25] = 0;                                                  // Dummy                 
                c[26] = (page>>8) & 0xFF;                                   // Page address to write H
                c[27] = (page>>0) & 0xFF;                                   // Page address to write L                                                                      // 26-27 Page address to write, updated later
                c[28] = SPI_CMD_DESELECT;
                c[29] = SPI_CMD_SELECT;
                c[30] = SPI_CMD_SPINAND_WAIT;                               // Check busy
                c[31] = SPI_CMD_DESELECT;
                
                pages_to_write++;                                           // Increase pages to be written
                page++;                                                     // Increase current page
                d += page_size;                                             // Increase input buffer
                j=0;
                if(++i>=block_size){ break; }                               // Done with this page
            }
            else if (++j==page_size){                                       // Increase scan, if reached end of page, it's empty, skip
                page++;                                                     // Increase page
                d += page_size;                                             // Increase input buffer                    
                j=0;                                                        // Reset counter
            }
        }
        cbuf[pages_to_write*TX_CMD_SZ] = SPI_CMD_END;                       // Finish cmd
        fel_write(ctx, pdat.swapbuf, dbuf, pages_to_write * page_size);     // Transfer TX buffer        
        fel_chip_spi_run(ctx, cbuf, (TX_CMD_SZ*pages_to_write)+1);          // Run Command buffer
        progress_update(&p, (page-last_page)*page_size);                    // Update progress
        last_page = page;
    }
    
    progress_stop(&p);
    return 1;  
}

int dso2d_dump_regs(struct xfel_ctx_t * ctx)
{
    struct spinand_pdata_t pdat;
    uint8_t s1,s2,s3;  
    const char * status_str[] = {
                                    "\r\n"                                                          // Gigadevice
                                    "Status 1: 0x%02X\r\n"
                                    "BRWD\tRES\tBP2\tBP1\tBP0\tINV\tCMP\tRES\r\n"
                                    "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\r\n\n"            
                                    "Status 2: 0x%02X\r\n"
                                    "OTP-PRT\tOTP-EN\tRES\tECC-EN\tBPL\tRES\tRES\tQE\r\n"
                                    "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\r\n\n"            
                                    "Status 3: 0x%02X\r\n"         
                                    "RES\tRES\tECCS1\tECCS0\tP-FAIL\tE-FAIL\tWEL\tOIP\r\n"
                                    "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\r\n\n",
                                    
                                    "\r\n"                                                          // Windbond
                                    "Status 1: 0x%02X\r\n"
                                    "SRP0\tBP3\tBP2\tBP1\tBP0\tTB\tWP-E\tSRP1\r\n"
                                    "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\r\n\n"            
                                    "Status 2: 0x%02X\r\n"
                                    "OTP-L\tOTP-E\tSR1-L\tECC-E\tBUF\tRES\tRES\tRES\r\n"
                                    "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\r\n\n"            
                                    "Status 3: 0x%02X\r\n"         
                                    "RES\tLUT-F\tECC-1\tECC-0\tP-FAIL\tE-FAIL\tWEL\tBUSY\r\n"
                                    "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\r\n\n",
    };
                                    
    if(!spinand_helper_init(ctx, &pdat, 0)) { return 0; }
    if(!spinand_get_feature(ctx, &pdat, OPCODE_FEATURE_PROTECT, &s1)) { return 0; }
    if(!spinand_get_feature(ctx, &pdat, OPCODE_FEATURE_CONFIG,  &s2)) { return 0; }
    if(!spinand_get_feature(ctx, &pdat, OPCODE_FEATURE_STATUS,  &s3)) { return 0; }
    
    uint8_t dev;
    switch(pdat.info.id.val[0]){
        case 0xC8:
            dev = 0;
            break;
        
        case 0xEF:
            dev = 1;
            break;
        
        default:
            printf("\r\nDevice '%s' not implemented\r\n", pdat.info.name);
            return 0;
    };
    printf("\r\nDevice: '%s'\r\n", pdat.info.name);
    printf(status_str[dev],            
            s1, (s1&0x80)&&1, (s1&0x40)&&1, (s1&0x20)&&1, (s1&0x10)&&1, (s1&0x8)&&1, (s1&0x4)&&1, (s1&0x2)&&1, (s1&0x1)&&1,
            s2, (s2&0x80)&&1, (s2&0x40)&&1, (s2&0x20)&&1, (s2&0x10)&&1, (s2&0x8)&&1, (s2&0x4)&&1, (s2&0x2)&&1, (s2&0x1)&&1,
            s3, (s3&0x80)&&1, (s3&0x40)&&1, (s3&0x20)&&1, (s3&0x10)&&1, (s3&0x8)&&1, (s3&0x4)&&1, (s3&0x2)&&1, (s3&0x1)&&1 );
        
  return 1;
}