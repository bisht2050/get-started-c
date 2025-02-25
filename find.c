// find.c
#include "URI.h"
#include <mongoc/mongoc.h>

int main(int argc, char const *argv[]) {
  // your MongoDB URI connection string
  const char *uri_string = MY_MONGODB_URI;
  // MongoDB URI created from above string
  mongoc_uri_t *uri;
  // MongoDB Client, used to connect to the DB
  mongoc_client_t *client;

  // Error management
  bson_error_t error;

  mongoc_collection_t *collection;
  char **collection_names;
  unsigned i;

  // Query object
  bson_t *query;
  mongoc_cursor_t *cursor;

  char *str;

  /*
   * Required to initialize libmongoc's internals
   */
  mongoc_init();

  /*
   * Safely create a MongoDB URI object from the given string
   */
  uri = mongoc_uri_new_with_error(uri_string, &error);
  if (!uri) {
    fprintf(stderr,
            "failed to parse URI: %s\n"
            "error message:       %s\n",
            uri_string, error.message);
    return EXIT_FAILURE;
  }

  /*
   * Create a new client instance, here we use the uri we just built
   */
  client = mongoc_client_new_from_uri(uri);
  if (!client) {
    puts("Error connecting!");
    return EXIT_FAILURE;
  }

  /*
   * Register the application name so we can track it in the profile logs
   * on the server. This can also be done from the URI (see other examples).
   */
  mongoc_client_set_appname(client, "connect-example");

  /*
   * Get a handle on the database "db_name" and collection "coll_name"
   */
  collection = mongoc_client_get_collection(client, "sample_mflix", "movies");

  query = bson_new();

  // All movies from 1984!
  BSON_APPEND_INT32(query, "year", 1984);
  cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);

  while (mongoc_cursor_next(cursor, &query)) {
    str = bson_as_canonical_extended_json(query, NULL);
    printf("%s\n\n", str);
    bson_free(str);
  }

  /*
   * Release our handles and clean up libmongoc
   */

  bson_destroy(query);

  mongoc_collection_destroy(collection);
  mongoc_uri_destroy(uri);
  mongoc_client_destroy(client);
  mongoc_cleanup();

  return EXIT_SUCCESS;
}
