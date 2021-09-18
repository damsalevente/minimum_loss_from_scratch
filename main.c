#include <stdio.h>
#include <stdlib.h>

#define MAP_SIZE 0xffff
#define KEY_BUF_SIZE 256

typedef enum map_ret_type { SUCC = 0, ERR } map_ret_type;

typedef enum check { CHECK, NO_CHECK } check;

typedef struct _dict {
  int *map;
  int size;
  int *keys;
  int bufsize;
  int mapsize;

  map_ret_type (*init)(struct _dict *);
  map_ret_type (*get)(struct _dict *, int, int *, int);
  map_ret_type (*insert)(struct _dict *, int, int);
  map_ret_type (*delete)(struct _dict *, int);
  map_ret_type (*destroy)(struct _dict *);
} dict;

map_ret_type map_init(dict *map) {
  map->size = 0;               /* how many elements do we have */
  map->bufsize = KEY_BUF_SIZE; /* buffered input, so no need for realloc after
                                  every insert */
  map->mapsize = MAP_SIZE;
  map->keys = (int *)malloc(map->bufsize * sizeof(int));
  map->map = (int *)malloc(MAP_SIZE * sizeof(int));
  if (map->map == NULL) {
    return ERR;
  }
  if (map->keys == NULL) {
    return ERR;
  }
  return SUCC;
}

map_ret_type map_free(dict *map) {
  free(map->map);
  free(map->keys);
  return SUCC;
}
map_ret_type map_insert(dict *map, int key, int value) {
  map->map[key] = value;
  if (map->keys[0] == 0) {
    map->keys[map->size++] = key;
  } else {
    int *tmp = (int *)malloc((map->size + 1) * sizeof(int));
    int idx = 0;
    while (map->keys[idx] < key && idx < map->size) {
      tmp[idx] = map->keys[idx];
      idx++;
    }
    tmp[idx++] = key;
    for (int i = idx; i < map->size + 1; i++) {
      tmp[i] = map->keys[i - 1];
    }
    map->size++;
    free(map->keys);
    map->keys = tmp;
  }
  /* keys buffer kind of full */
  if (2 * map->size > map->bufsize) {
    map->bufsize *= 2;
    int *tmp = (int *)malloc(map->bufsize * sizeof(int));

    for (int i = 0; i < map->size; i++) {
      tmp[i] = map->keys[i];
    }
    free(map->keys);
    map->keys = tmp;
  }
  /* map buffer kind of full */
  if (2 * key > map->mapsize) {
    int *tmp = (int *)malloc(2 * map->mapsize * sizeof(int));
    /* copy only the keys */
    for (int i = 0; i < map->size; i++) {
      tmp[map->keys[i]] = map->map[map->keys[i]];
    }
    free(map->map);
    map->map = tmp;
    map->mapsize *= 2;
  }
  return SUCC;
}

map_ret_type map_delete(dict *map, int key) {
  map->map[key] = 0;
  int found = 0;
  for (int i = 0; i < map->size - 1; i++) {
    if (found) {
      map->keys[i] = map->keys[i + 1];
    }
    if (map->keys[i] == key) {
      found = 1;
      map->keys[i] = map->keys[i + 1];
    }
  }
  if (!found) {
    return ERR;
  }
  map->size--;
  return SUCC;
}

map_ret_type map_get(dict *map, int key, int *result, int check_key) {
  *result = map->map[key];

  if (check_key) {
    for (int i = 0; i < map->size; i++) {
      if (map->keys[i] == key) {
        return SUCC;
      }
    }
    return ERR;
  }
  return SUCC;
}

static dict int_dict = {
    .map = NULL,
    .size = 0,
    .keys = NULL,
    .bufsize = 0,
    .mapsize = 0,
    .init = map_init,
    .get = map_get,
    .insert = map_insert,
    .delete = map_delete,
    .destroy = map_free,
};

int problem(int *costs, int size) {
  dict *d = &int_dict;
  int answer = 0xFFFFF;
  d->init(d);
  /* costs contains all the prices in chronological order */
  for (int i = 0; i < size; i++) {
    /* insert sorted */
    d->insert(d, costs[i], i);
  }
  /* walk throught the keys, i.e the prices */
  for (int i = 0; i < d->size - 1; i++) {
    /* buytime is the bigger one has to be smaller than 0 */
    int is_buy_first =
        (d->map[d->keys[i]] - d->map[d->keys[i + 1]]) < 0 ? 0 : 1;
    if (is_buy_first) {
      /* since it's sorted, the next element will be bigger */
      if (d->keys[i + 1] - d->keys[i] < answer) {
        answer = d->keys[i + 1] - d->keys[i];
      }
    }
  }
  return answer;
}

int main() {
  int costs[] = {20, 15, 8, 2, 12, 0xfff1};
  int result = 0;
  result = problem(costs, 5);
  printf("%d\n", result);

  return 0;
}
