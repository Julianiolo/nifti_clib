#ifndef NIFTI1_TOOL_H
#define NIFTI1_TOOL_H

#define NT_CMD_LEN 2048

#ifndef NIO_API
  #if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(NIFTIIO_BUILD_SHARED)
      #ifdef __GNUC__
        #define NIO_API __attribute__ ((dllexport))
      #else
        #define NIO_API __declspec((dllexport))
      #endif
    #elif defined(NIFTIIO_USE_SHARED)
      #ifdef __GNUC__
        #define NIO_API __attribute__ ((dllimport))
      #else
        #define NIO_API __declspec((dllimport))
      #endif
    #endif
  #elif (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
    #define NIO_API __attribute__ ((visibility ("default")))
  #else
    #define NIO_API
  #endif
#endif

typedef struct{
   int           len;
   const char ** list;
} str_list;

typedef struct{
   int     len;
   int   * list;
} int_list;

typedef struct{
            /* action options (flags) */
   int      check_hdr,  check_nim;
   int      diff_hdr,   diff_nim;
   int      disp_hdr,   disp_nim, disp_ana;
   int      disp_exts,  add_exts, rm_exts;
   int      mod_hdr,    mod_nim;
   int      swap_hdr,   swap_ana, swap_old;

   int      strip;               /* strip extras from dataset(s)  */
   int      cbl, cci;            /* -copy_XXX option flags        */
   int      dts, dci, dci_lines; /* display collapsed img flags   */
   int      make_im;             /* create a new image on the fly */
   int      ci_dims[8];          /* user dims list (last 7 valid) */
   int      new_dim[8];          /* user dim list for new image   */
   int      new_datatype;        /* datatype for new image        */
   int      debug, keep_hist;    /* debug level and history flag  */
   int      overwrite;           /* overwrite flag                */
   char *   prefix;              /* for output file               */
   str_list elist;               /* extension strings             */
   int_list etypes;              /* extension type list           */
   str_list flist;               /* fields (to display or modify) */
   str_list vlist;               /* values (to set fields to)     */
   str_list infiles;             /* input files                   */
   char     command[NT_CMD_LEN]; /* for inserting the command     */
} nt_opts;

#define USE_SHORT      1
#define USE_FULL       2
#define USE_HIST       3
#define USE_FIELD_HDR  4
#define USE_FIELD_NIM  5
#define USE_FIELD_ANA  6
#define USE_DTYPES     7
#define USE_VERSION    8

#define CHECK_NEXT_OPT(n,m,str)                                       \
   do { if ( (n) >= (m) ) {                                           \
           fprintf(stderr,"** option '%s': missing parameter\n",str); \
           fprintf(stderr,"   consider: 'nifti_tool -help'\n");       \
           return 1;      }                                           \
      } while(0)

#define CHECK_NEXT_OPT_MSG(n,m,str,msg)                               \
   do { if ( (n) >= (m) ) {                                           \
           fprintf(stderr,"** option '%s': %s\n",str,msg);            \
           fprintf(stderr,"   consider: 'nifti_tool -help'\n");       \
           return 1;      }                                           \
      } while(0)

/*----------------------------------------------------------------------
 * this structure and definitions will be used to process the nifti_1_header
 * and nifti_image structure fields (actions disp, diff, mod)
 *----------------------------------------------------------------------*/

#define NT_FIELD_NAME_LEN  20       /* more than length of longest name */
#define NT_HDR_NUM_FIELDS  43       /* in the nifti_1_header struct     */
#define NT_ANA_NUM_FIELDS  47       /* in the  nifti_analyze75 struct   */
#define NT_NIM_NUM_FIELDS  63       /* in the nifti_image struct        */
#define NT_DT_STRING      -0xfff    /* some strange number to abuse...  */
#define NT_DT_POINTER     -0xfef    /* some strange number to abuse...  */
#define NT_DT_CHAR_PTR    -0xfee    /* another...                       */
#define NT_DT_EXT_PTR     -0xfed    /* and another...                   */

typedef struct {
   int  type;                    /* one of the DT_* types from nifti1.h */
   int  offset;                  /* bytes from the start of the struct  */
   int  size;                    /* size of one element type            */
   int  len;                     /* number of elements                  */
   char name[NT_FIELD_NAME_LEN]; /* actual structure name used          */
} field_s;

/* for computing the offset from the start of the struct */
#define NT_OFF(str,field) ((int)( ((char *)&str.field) - ((char *)&str) ))

/* call fill_field() for a single type, name and number of elements */
/* nstr is the base struct, and fldp is a field pointer */
#define NT_SFILL(nstr,fldp,type,name,num,rv) do{                   \
           rv=fill_field(fldp,type,NT_OFF(nstr,name),num,#name);   \
           fldp++; } while (0)

#define NT_MAKE_IM_NAME "MAKE_IM"

/*----------------------------------------------------------------------*/
/*-----  prototypes  ---------------------------------------------------*/
/*----------------------------------------------------------------------*/
NIO_API int    act_add_exts   ( nt_opts * opts );
NIO_API int    act_cbl        ( nt_opts * opts );  /* copy brick list */
NIO_API int    act_cci        ( nt_opts * opts );  /* copy collapsed dimensions */
NIO_API int    act_check_hdrs ( nt_opts * opts );  /* check for valid hdr or nim */
NIO_API int    act_diff_hdrs  ( nt_opts * opts );
NIO_API int    act_diff_nims  ( nt_opts * opts );
NIO_API int    act_disp_ci    ( nt_opts * opts );  /* display general collapsed data */
NIO_API int    act_disp_exts  ( nt_opts * opts );
NIO_API int    act_disp_hdrs  ( nt_opts * opts );
NIO_API int    act_disp_nims  ( nt_opts * opts );
NIO_API int    act_disp_anas  ( nt_opts * opts );
NIO_API int    act_disp_ts    ( nt_opts * opts );  /* display time series */
NIO_API int    act_mod_hdrs   ( nt_opts * opts );
NIO_API int    act_mod_nims   ( nt_opts * opts );
NIO_API int    act_swap_hdrs  ( nt_opts * opts );
NIO_API int    act_rm_ext     ( nt_opts * opts );
NIO_API int    act_strip      ( nt_opts * opts );  /* strip extras from datasets */


NIO_API field_s    * get_hdr_field( const char * fname, int show_fail );
NIO_API field_s    * get_nim_field( const char * fname, int show_fail );
NIO_API const char * field_type_str (int type);

NIO_API int diff_hdrs     (nifti_1_header *s0, nifti_1_header *s1, int display);
NIO_API int diff_hdrs_list(nifti_1_header *s0, nifti_1_header *s1, str_list *slist,
                               int display);
NIO_API int diff_nims     (nifti_image *s0,nifti_image *s1,        int display);
NIO_API int diff_nims_list(nifti_image *s0,nifti_image *s1,str_list *slist,int display);

NIO_API int add_int          (int_list * ilist, int val);
NIO_API int add_string       (str_list * slist, const char * str);
NIO_API int check_total_size ( const char *mesg, field_s *fields, int nfields, int tot_size);
NIO_API int clear_float_zeros( char * str );
NIO_API int diff_field       (field_s *fieldp, void * str0, void * str1, int nfields);
NIO_API int disp_nifti1_extension( const char *mesg, nifti1_extension * ext, int maxlen);
NIO_API int disp_field       ( const char *mesg,field_s *fieldp,void *str,int nfields,int header);
NIO_API int disp_field_s_list( const char *mesg, field_s *, int nfields);
NIO_API int disp_nt_opts     ( const char *mesg, nt_opts * opts);
NIO_API int disp_raw_data    (void * data, int type, int nvals, char space,int newline);
NIO_API int fill_cmd_string  (nt_opts * opts, int argc, char * argv[]);
NIO_API int fill_field       (field_s *fp, int type, int offset, int num, const char *name);
NIO_API int fill_hdr_field_array(field_s * nh_fields);
NIO_API int fill_nim_field_array(field_s * nim_fields);
NIO_API int fill_ana_field_array(field_s * ah_fields);
NIO_API int modify_all_fields(void *basep, nt_opts *opts, field_s *fields, int flen);
NIO_API int modify_field     (void * basep, field_s * field, const char * data);
NIO_API int process_opts     (int argc, char * argv[], nt_opts * opts);
NIO_API int remove_ext_list  (nifti_image * nim, const char ** elist, int len);
NIO_API int usage            (const char * prog, int level);
NIO_API int use_full         (const char * prog);
NIO_API int verify_opts      (nt_opts * opts, char * prog);
NIO_API int write_hdr_to_file(nifti_1_header * nhdr, const char * fname);

/* wrappers for nifti reading functions (allow MAKE_IM) */
NIO_API nifti_image    * nt_image_read (nt_opts * opts, const char * fname, int doread);
NIO_API nifti_image    * nt_read_bricks(nt_opts * opts, const char * fname, int len,
                                            int * list, nifti_brick_list * NBL);
NIO_API nifti_1_header * nt_read_header(nt_opts * opts, const char * fname, int * swapped,
                                            int check);


#endif  /* NIFTI1_TOOL_H */
