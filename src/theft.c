#include <string.h>
#include <assert.h>

#include "theft.h"
#include "theft_types_internal.h"
#include "theft_run.h"

#include "theft_bloom.h"
#include "theft_mt.h"

/* Forward references.
 * These functions are not static because some tests
 * need to construct theft instances. */
struct theft *theft_init(void);
void theft_free(struct theft *t);

/* Initialize a theft test runner, with the configuration
 * in CFG. If CFG is NULL, a default will be used.
 *
 * Returns a NULL if malloc fails or the provided configuration
 * is invalid. */
struct theft *theft_init(void) {
    struct theft *t = malloc(sizeof(*t));
    if (t == NULL) { return NULL; }
    memset(t, 0, sizeof(*t));

    t->prng.mt = theft_mt_init(DEFAULT_THEFT_SEED);
    if (t->prng.mt == NULL) {
        free(t);
        return NULL;
    } else {
        t->out = stdout;
        return t;
    }
}

/* Change T's output stream handle to OUT. (Default: stdout.) */
void theft_set_output_stream(struct theft *t, FILE *out) {
    t->out = out;
}

/* Run a series of randomized trials of a property function.
 *
 * Configuration is specified in CFG; many fields are optional.
 * See the type definition in `theft_types.h`. */
enum theft_run_res
theft_run(const struct theft_run_config *cfg) {
    struct theft *t = theft_init();
    if (t == NULL) {
        return THEFT_RUN_ERROR;
    }

    if (t == NULL || cfg == NULL || cfg->fun == NULL) {
        theft_free(t);
        return THEFT_RUN_ERROR_BAD_ARGS;
    }

    enum theft_run_res res = theft_run_trials(t, cfg);
    theft_free(t);
    return res;
}

/* Free a property-test runner. */
void theft_free(struct theft *t) {
    if (t->bloom) {
        theft_bloom_free(t->bloom);
        t->bloom = NULL;
    }
    theft_mt_free(t->prng.mt);
    free(t);
}
