/*
** FILE: wparse.h
**
** PURPOSE: Functions for parsing tokens and generating ASTs
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#ifndef WPARSE_H
#define WPARSE_H

#include "winter.h"
#include "wlex.h"
#include "wtable.h"

typedef struct ast_node_t {
	token_type_t type;
	object_t value;
	
	int numNodes;
	struct ast_node_t **nodes;
} ast_node_t;

ast_node_t *generateTreeThing(const char *source);
ast_node_t *execute(ast_node_t *tree);

#endif
