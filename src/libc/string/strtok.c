
#include <string.h>

static char *pdelim_save;

char *strtok(char *restrict s1, const char *restrict s2)
{
    char *ptoken;
    char *pdelim;

    /* Let's follow the C standard step by step */
    if (!s1)
    {
        /* Each subsequent call, with a null pointer as the value of the first
         * argument, starts searching from the saved pointer..
         */
        if (!pdelim_save)
        {
            /* We have previously exhausted the string - see below where
             * pdelim_save is set to NULL
             */
            return NULL;
        }
        s1 = pdelim_save;
    }

    /* ..searches the string pointed to by s1 for the first character that
     * _is not_ contained in the current separator string pointed to by s2
     */
    ptoken = s1 + strspn(s1, s2);
    if (ptoken[0] == '\0')
    {
        /* If no such character is found, then there are no tokens in the string
         * pointed to by s1 and the strtok function returns a null pointer.
         */
        pdelim_save = NULL;
        return NULL;
    }

    /* If such a character is found, it is the start of the first token.
     * The strtok function then searches from there for a character that
     * _is_ contained in the current separator string.
     */
    pdelim = ptoken + strcspn(ptoken, s2);
    if (pdelim[0] == '\0')
    {
        /* If no such character is found, the current token extends to the end
         * of the string pointed to by s1, and subsequent searches for a token
         * will return a null pointer.
         */
        pdelim_save = NULL;
    }
    else
    {
        /* If such a character is found, it is overwritten by a null character,
         * which terminates the current token.
         */
        pdelim[0] = '\0';
        pdelim_save = pdelim + 1;
    }

    /* The strtok function returns a pointer to the first character of a token,
     * or a null pointer if there is no token.
     */
    return ptoken;
}
