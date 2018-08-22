#include <regex.h>  
#include <stdlib.h>
#include <stdio.h>

#include "url_parser.h"      


int match(char* string){

	regex_t regex;

	int reti;
	char msgbuf[100];

	/* Compile regular expression */
	reti = regcomp(&regex, "HTTP", 0);
	if (reti) {
	    fprintf(stderr, "Could not compile regex\n");
	    exit(1);
	}

	/* Execute regular expression */
	reti = regexec(&regex, string, 0, NULL, 0);
	if (!reti) {
		printf("sddad\n");
	    return 1;
	}
	else if (reti == REG_NOMATCH) {
	    return 0;
	}
	else {
	    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
	    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
	    exit(1);
	}

	/* Free memory allocated to the pattern buffer by regcomp() */
	regfree(&regex);

}