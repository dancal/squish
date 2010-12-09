#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <gopt.h>

int main(int argc, const char **argv){
   // make configurable later
  const char *separator = " "; 
  char delimiter = ' ';

  FILE *input = stdin;
  FILE *output = stdout;

  bool usage_fail = false;

  void *options = gopt_sort(&argc, argv, gopt_start(
    gopt_option('h', 0, gopt_shorts('h'), gopt_longs("help")),
    gopt_option('d', GOPT_ARG, gopt_shorts('d'), gopt_longs("delimiter")),
    gopt_option('s', GOPT_ARG, gopt_shorts('s'), gopt_longs("separator")),
    gopt_option('o', GOPT_ARG, gopt_shorts('o'), gopt_longs("output"))
  ));

  if( gopt(options, 'h')){
    puts("Usage: squish [options] [target]");
    puts("Options are:");
    puts("  -h, --help: You're looking at it");
    puts("  -d, --delimiter: Specify the character that marks the end of the initial key. Defaults to ' '");
    puts("  -s, --separator: Specify the separator between values. Defaults to delimiter"); 
    puts("  -o, --output: The file to write output to. Defaults to stdout");
    
    exit(0);
  }

  const char *long_delimiter = NULL;
  if(gopt_arg(options, 'd', &long_delimiter)){
    if(strlen(long_delimiter) != 1){
      fprintf(stderr, "squish: delimiter must be a single character\n");
      usage_fail = true;
    }

    delimiter = long_delimiter[0];
    separator = long_delimiter;
  }

  gopt_arg(options, 's', &separator);


  const char *output_file = NULL;
  if(gopt_arg(options, 'o', &output_file) && strcmp(output_file, "-" )){
    output = fopen(output_file, "w");
    if(!output){
      fprintf(stderr, "squish: Could not open output file %s for writing\n",  output_file);
      usage_fail = true;
    }
  }

  if(argc > 2){
    fprintf(stderr, "Too many arguments: ");
    for(int i = 1; i < argc; i++){
      fprintf(stderr, "%s ", argv[i]);
    }
    fprintf(stderr, "\n");
    usage_fail = true;
  }

  if(argc == 2){
    input = fopen(argv[1], "r");
    if(!input){
      fprintf(stderr, "squish: Could not open input file %s for reading\n", argv[1]);
      usage_fail = true;
    }
  }

  gopt_free(options);
  
  if(usage_fail) exit(1);

  char c;

  int current_buffer_length = -1; 
  int max_buffer_length = 1024;
  char* buffer = malloc(max_buffer_length);

  bool first_line = true;

  while(true){ // loop over lines
    // first read the key
    bool key_changed = false;
    int index_into_key = 0;
    while(true){
      if(index_into_key >= max_buffer_length - 1){
        max_buffer_length *= 2;
        buffer = realloc(buffer, max_buffer_length);
      }

      c = fgetc(input);


      if(c == delimiter || c == '\n' || c == EOF) {
        if(index_into_key < current_buffer_length) key_changed = true;
        current_buffer_length = index_into_key;
        buffer[index_into_key] = '\0';
        break; 
      }

      key_changed = key_changed || current_buffer_length < 0 || (buffer[index_into_key] != c);

      buffer[index_into_key] = c;


      index_into_key++;
    }

    // If the key is changed we start a new line and then print it
    if(key_changed){
      if(first_line){
        fprintf(output, "%s", buffer);
        first_line = false;
      } else fprintf(output, "\n%s", buffer);
    }

    if(c == EOF) break;
    if(c == '\n') continue;

    if(key_changed){
      fputc(delimiter, output);
    } else {
      fprintf(output, "%s", separator);
    }   
 

    // now write the rest
    while(true){
      c = fgetc(input);
      if(c == EOF || c == '\n') break;
      fputc(c, output);
    }
    if(c == EOF) break;
  }

  free(buffer);
}
