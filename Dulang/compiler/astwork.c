//
//  astwork.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 02.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "astgen.h"

#include "compilerr.h"
#include <stdarg.h>



lexem get_current   (lexem_iterator * iter){
    if(iter->currlexem >= iter->lstream.lexemcount){
        lexem ret_l = {NONE, 0, 0, 0};
        return ret_l;
    }
    return iter->lstream.lexems[iter->currlexem];
}

lexem get_next      (lexem_iterator * iter){
    iter->currlexem ++;
    return get_current(iter);
}

lexem get_next_pos  (lexem_iterator * iter, int pos){
    if(iter->currlexem + pos >= iter->lstream.lexemcount){
        lexem ret_l = {NONE, 0, 0, 0};
        return ret_l;
    }
    return iter->lstream.lexems[iter->currlexem + pos];
}
int is_statement_terminating( lexem l ){
    return l.type == NONE || (l.type == SPECIAL && (l.special_type == EOL || l.special_type == Cbrace));
}
int lexem_occures_till_eol(lexem_iterator * iter, enum splexems type){
    int pos = -1, is_satisfying = 0;
    lexem l_next;
    do{
        pos++;
        l_next = get_next_pos(iter, pos);
        is_satisfying = l_next.type == SPECIAL && l_next.special_type == type;
        if(is_satisfying){
            return 1;
        }
    } while( !is_statement_terminating(l_next));
    return 0;
}




astnode * astnode_new  ( enum astnodetype type, int capacity, int children, ... ){
    va_list va;
    va_start(va, children);
    return astnode_val(type, capacity, children, va);
}

void ast_add_child (astnode * parent, astnode * child){
    if(parent->childrencount ++ == parent->childrencap){
        parent->childrencap *= 2;
        parent->children = (astnode**)realloc(parent->children, parent->childrencap);
    }
    parent->children[ parent->childrencount - 1 ] = child;
    child->parent = parent;
}

astnode * astnode_val  ( enum astnodetype type, int capacity, int children, va_list va_args){
    astnode * result = (astnode*)malloc(sizeof(astnode));
    result->type = type;
    result->children = (astnode**)malloc(sizeof(astnode*)*capacity);
    result->childrencap = capacity;
    result->childrencount = 0;
    result->place_needed = 0;
    for(int i = 0; i < children; ++i){
        ast_add_child(result, va_arg(va_args, astnode*));
    }
    result->node_type = 0;
    return result;
}

void destroy_astnode(astnode * node){
    for(int i = 0; i < node->childrencount; i++){
        destroy_astnode(node->children[i]);
    }
    if(node->childrencount)free(node->children);
    if(node->type == STRLIT || node->type == NAME){
        //free(node->val);
    }
    free(node);
}
