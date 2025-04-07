#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int
main ()
{
  // Starter HTML (to avoid copy-and-paste annoynances):
  printf ("<html lang=\"en\">\n");
  printf ("  <head>\n");
  printf ("    <title>File Hash Database</title>\n");
  printf ("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn."
          "com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\""
          "sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/"
          "iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\" />\n");
  printf ("  </head>\n\n");
  
  // These variables can be read from the environment using getenv().
  char *db = NULL;
  char *record = NULL;
  char *hash = NULL;
  char *query = NULL;
  // This is an HTML comment. It's useful for debugging to see if your
  // environment variables got through.


  // TODO [PART]: Read the data/data.txt file and produce an HTML table
  // to match the output in the cgi-src/tests/expected/PART_show_all.txt
  // file. Note that each line in the data.txt file should correspond to
  // two "col" divs, and all but the last line should be followed by a
  // "w-100" div. The overall framework of the body should look like this
  // (with exactly 2 spaces before the opening < character of each line):
  //    <body>
  //      <div class="container">
  //        <br />
  //        <h2 class="mb-0">Database Records</h2>
  //        <div class="row">
  //          <div class="col py-md-2 border bg-light">PART_hello.txt</div>
  //          <div class="col py-md-2 border bg-light">94079f...</div>
  //          <div class="w-100"></div>
  //          <div class="col py-md-2 border bg-light">PART_show_all.txt</div>
  //          <div class="col py-md-2 border bg-light">9e5543...</div>
  //        </div>
  //      </div>
  //    </body>

  query = getenv ("QUERY_STRING");

  if (query != NULL) // if query string is set
    {
      char *key = strtok (query, "=");
      while (key != NULL)
      {
        char *value = strtok (NULL, "&"); 
        if (strncmp ("db", key, 2) == 0)
          db = value;
        else if (strncmp ("record", key, 6) == 0)
          record = value;
        else if (strncmp ("hash", key, 4) == 0)
          hash = value;
        key = strtok (NULL, "=");
      }
    }
  else // looking for environment variables independently if query string is not set
    {
      db = getenv ("db");
      //printf ("DB: %s\n", db);
      record = getenv ("record");
      hash = getenv ("hash");
    }
    
  printf ("  <!-- Environment variables:\n");
  printf ("       db: %s\n", db);
  printf ("       record: %s\n", record);
  printf ("       hash: %s\n", hash);
  printf ("       query: %s\n", query);
  printf ("    -->\n\n");

  FILE* file;
  char buf[1024];
  if (db != NULL) //opening the specified record
    {
      /*int n = snprintf (buffer , 0, "data/%s", record);
      realloc (record, n+1);
      snprintf (buffer , n+1, "data/%s", record);
      record = strdup (buffer);*/

      char *dbname = "data/";
      printf ("DBName: %s\n", dbname);
      strcat (dbname, db);
      dbname = strdup (dbname);

      file  = fopen (dbname, "r");
    }
  else 
    {
      //printf ("Opening data/data.txt\n");     
      file  = fopen ("data/data.txt", "r");
    }
  printf ("  <body>\n    <div class=\"container\">\n      <br />\n      <h2 class=\"mb-0\">Database Records</h2>\n      <div class=\"row\">\n");
  
  int recordNum = 1;
  char* r = fgets(buf, sizeof(buf), file);
  while(r != NULL)
    {
      char* hashRead = strtok (buf, " ");
      if (record == NULL)
        {
          char* filename = strtok (NULL, " ");
          filename [strlen(filename)-1] = '\0';
          printf ("        <div class=\"col py-md-2 border bg-light\">%s</div>\n", filename);
          printf ("        <div class=\"col py-md-2 border bg-light\">%s</div>\n", hashRead);
          r = fgets(buf, sizeof(buf), file);
        }
      else if (atoi (record) == recordNum)
        {
          if (strcmp (hash, hashRead) != 0)
            { 
              char buff[1024];
              int n = snprintf (buff , 0, "%s <span class=\"badge badge-danger\">MISMATCH</span>", hashRead);
              realloc (hash, n+1);
              snprintf (buff , n+1, "%s <span class=\"badge badge-danger\">MISMATCH</span>", hashRead);
              hashRead = strdup (buff);
            }
          r = NULL;
        }

      if (r != NULL)
      	{
      	  printf ("        <div class=\"w-100\"></div>\n");
      	}
    }
  fclose (file);

  printf ("      </div>\n    </div>\n  </body>\n");
	
  // TODO [MIN]: Once you have the basic structure working, extend it to
  // read in environment variables (db, record, hash, and QUERY_STRING).
  // From a logic standpoint, if QUERY_STRING is set, use that and split it
  // apart at the & character. For example, the QUERY_STRING might look like:
  //   db=foo.txt&record=2&hash=9e5543354d4592db8272b3c3e14953770df88ba3
  // If the QUERY_STRING is not set, look for the db, record, and hash
  // environment variables independently. If the hash variable is set,
  // compare its value with the hash value for the specified record. (If
  // record is not set, then ignore the hash variable.) If the hash does not
  // match, add this code just after the hash value from the database (put
  // a space before the <span and no space between </span></div>):
  //    <span class="badge badge-danger">MISMATCH</span>

  printf ("\n</html>\n");

  return 0;
}
