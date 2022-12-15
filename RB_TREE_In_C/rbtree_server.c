#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "rbtree.h"

rbtree_t* create_rbtree()
{
    rbtree_t* p_rbtree = (rbtree_t*)xcalloc(1,sizeof(rbtree_t));
    p_rbtree->N = 0;
    p_rbtree->nil = (rbnode_t*)xcalloc(1,sizeof(rbnode_t));
    p_rbtree->nil->data = -1;
    p_rbtree->nil->color = BLACK;
    p_rbtree->nil->left = NULL;
    p_rbtree->nil->right = NULL;
    p_rbtree->nil->parent = NULL;
    p_rbtree->root_node = p_rbtree->nil;
    return (p_rbtree);
}

status_t destroy_rbtree(rbtree_t** pp_rbtree)
{
    rbtree_t* p_rbtree = *pp_rbtree;

    destroy_node_level(p_rbtree->root_node,p_rbtree->nil);
    free(p_rbtree->nil);
    free(p_rbtree);

    *pp_rbtree = NULL;
    return (SUCCESS);
}

void preorder_walk(rbtree_t* p_rbtree)
{
    printf("Preorder walk:");
    printf("[START]<->");
    preorder_node_walk(p_rbtree->root_node,p_rbtree->nil);
    puts("[END]");
}

void inorder_walk(rbtree_t* p_rbtree)
{
    printf("Inorder walk:");
    printf("[START]<->");
    inorder_node_walk(p_rbtree->root_node,p_rbtree->nil);
    puts("[END]");
}

void postorder_walk(rbtree_t* p_rbtree)
{
    printf("Postorder walk:");
    printf("[START]<->");
    postorder_node_walk(p_rbtree->root_node,p_rbtree->nil);
    puts("[END]");
}

status_t insert_into_rbtree(rbtree_t* p_rbtree, data_t new_data)
{
    rbnode_t* p_run = NULL;
    rbnode_t* z = NULL;

    z = get_rbnode(new_data,p_rbtree->nil);

    if(p_rbtree->root_node == p_rbtree->nil)
    {
        p_rbtree->root_node = z;
        p_rbtree->N += 1;
        rb_insert_fixup(p_rbtree, z);
        return (SUCCESS); 
    }

    p_run = p_rbtree->root_node;
    while(TRUE)
    {
        if(new_data <= p_run->data)
        {
            if(p_run->left == p_rbtree->nil)
            {
                p_run->left = z;
                z->parent = p_run;
                break;
            }
            else
                p_run = p_run->left;
        }
        else
        {
            if(p_run->right == p_rbtree->nil)
            {
                p_run->right = z;
                z->parent = p_run;
                break;
            }
            else
                p_run = p_run->right;
        }
    }

    p_rbtree->N += 1;
    rb_insert_fixup(p_rbtree, z);
    return (SUCCESS);
}

status_t remove_from_rbtree(rbtree_t* p_rbtree, data_t r_data)
{
    rbnode_t* z = p_rbtree->nil; /* Node to be deleted */
    rbnode_t* y = p_rbtree->nil; /* Inorder successor of node to be deleted
                                    in case 3, 4 for bst_remove */
    rbnode_t* x = p_rbtree->nil; /* To store address of root of subtree
                                    that requires fixing */
    color_t y_original_color;    /* to determine whether subtree rooted
                                    at x requires fixing */

    z = search_rb_node(p_rbtree, r_data);
    if(z == p_rbtree->nil)
        return (RB_DATA_NOT_FOUND);

    y = z;
    y_original_color = y->color;

    if(z->left == p_rbtree->nil)
    {
        y = z->right;
        rb_transplant(p_rbtree,z,z->right);
    }
    else if(z->right == p_rbtree->nil)
    {
        y = z->left;
        rb_transplant(p_rbtree,z,z->left);
    }
    else
    {
        y = get_rb_min_node(p_rbtree,z->right);
        y_original_color = y->color;
        x = y->right;
        if(y->parent == z)
            x->parent = y;
        else
        {
            rb_transplant(p_rbtree,y,y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(p_rbtree,z,y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if(y_original_color == BLACK && x != p_rbtree->nil)
        rb_delete_fixup(p_rbtree, x);

        return (SUCCESS);
}

status_t find_rb_data(rbtree_t* p_rbtree, data_t search_data)
{
    rbnode_t* p_find_node = p_rbtree->nil;
    p_find_node = search_rb_node(p_rbtree,search_data);
    return (p_find_node != p_rbtree->nil);
}

unsigned int get_rb_height(rbtree_t* p_rbtree)
{
    return get_height_nodelevel(p_rbtree,p_rbtree->root_node);
}

/*Implementation of Auxilary routines*/

rbnode_t* search_rb_node(rbtree_t* p_rbtree, data_t search_data)
{
    rbnode_t* p_run = p_rbtree->root_node;
    while(p_run != p_rbtree->nil)
    {
        if(search_data == p_run->data)
            return p_run;
        else if(search_data < p_run->data)
            p_run = p_run->left;
        else
            p_run = p_run->right;
    }

    return p_run;
}

void rb_insert_fixup(rbtree_t* p_rbtree, rbnode_t* z)
{
    rbnode_t* y = NULL;

    while(z->parent->color == RED)
    {
        if(z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;
            if(y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->right)
                {
                    z = z->parent;
                    left_rotate(p_rbtree,z);
                }
                z->parent->color = BLACK;
                z->parent->parent = RED;
                right_rotate(p_rbtree,z->parent->parent);
            }
        }
        else
        {
            y = z->parent->parent->left;
            if(y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                y->parent->parent = RED;
                z = z->parent->parent;
            }
            else
            {
                if(z = z->parent->left)
                {
                    z = z->parent;
                    right_rotate(p_rbtree,z);
                }
                z->parent->color = BLACK;
                z->parent->parent = RED;
                left_rotate(p_rbtree,z->parent->parent);
            }
        }
    }

    p_rbtree->root_node->color = BLACK;
}

void rb_delete_fixup(rbtree_t* p_rbtree, rbnode_t* x)
{
    rbnode_t* w = p_rbtree->nil;
    while(x != p_rbtree->root_node && x->color == BLACK)
    {
        if(x == x->parent->left)
        {
            w = x->parent->right;
            if(w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(p_rbtree,x->parent);
                w = x->parent->right;
            }

            if(w == p_rbtree->nil)
            {
                x = x->parent;
                continue;
            }

            if(w->right->color == BLACK && w->left->color == BLACK)
            {
                w->color = RED;     /* case 2 */
                x = x->parent;      /* case 2 */
            }
            else
            {
                if(w->right->color == BLACK)
                {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(p_rbtree,w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(p_rbtree,x->parent);
                x = p_rbtree->root_node;
            }
        }
        else
        {
            w = x->parent->left;
            if(w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(p_rbtree,x->parent);
                w = x->parent->left;
            }

            if(w == p_rbtree->nil)
            {
                x = x->parent;
                continue;
            }

            if(w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if(w->left->color == BLACK)
                {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(p_rbtree,w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(p_rbtree,x->parent);
                x = p_rbtree->root_node;
            }
        }
    }
}



void left_rotate(rbtree_t* p_tree, rbnode_t* x)
{
    rbnode_t* y = NULL;

    /* Part 1 */
    y = x->right;
    x->right = y->left;
    if(y->left != p_tree->nil)
        y->left->parent = x;

    /* Part 2 */
    y->parent = x->parent;
    if(x->parent == p_tree->nil)
        p_tree->root_node = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else if(x == x->parent->right)
        x->parent->right = y;

    /* Part 3 */
    x->parent = y;
    y->left = x;
}

/* Precondition: x->left != p_tree->nil */
void right_rotate(rbtree_t* p_tree, rbnode_t* x)
{
    rbnode_t* y = NULL;

    /* Part 1 */
    y = x->left;
    x->left = y->right;
    if(y->right != p_tree->nil)
        y->parent = x;

    /* Part 2 */
    y->parent = x->parent;
    if(x->parent == p_tree->nil)
        p_tree->root_node = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else if(x == x->parent->left)
        x->parent->left = y;

    /* Part 3 */
    y->right = x;
    x->parent = y;
}

void rb_transplant(rbtree_t* p_rbtree, rbnode_t* u, rbnode_t* v)
{
    if(u->parent == p_rbtree->nil)
        p_rbtree->root_node = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else if(u == u->parent->right)
        u->parent->right = v;
    
    if(v != p_rbtree->nil)
        v->parent = u->parent;
}

unsigned int get_height_nodelevel(rbtree_t* p_tree, rbnode_t* p_node)
{
    if(p_node == p_tree->nil)
        return 0;
    return (max(get_height_nodelevel(p_tree,p_node->left),
                 get_height_nodelevel(p_tree,p_node->right)) + 1);
}

void destroy_node_level(rbnode_t* p_root_node, rbnode_t* nil)
{
    if(p_root_node != nil)
    {
        destroy_node_level(p_root_node->left,nil);
        destroy_node_level(p_root_node->right,nil);
        free(p_root_node);
    }
}

void preorder_node_walk(rbnode_t*  p_rbnode, rbnode_t* nil)
{
    if(p_rbnode != nil)
    {
        printf("[%d]<->",p_rbnode->data);
        preorder_node_walk(p_rbnode->left,nil);
        preorder_node_walk(p_rbnode->right,nil);
    }
}

void inorder_node_walk(rbnode_t* p_rbnode, rbnode_t* nil)
{
    if(p_rbnode != nil)
    {
        inorder_node_walk(p_rbnode->left,nil);
        printf("[%d]<->",p_rbnode->data);
        inorder_node_walk(p_rbnode->right,nil);
    }
}

void postorder_node_walk(rbnode_t* p_rbnode, rbnode_t* nil)
{
    if(p_rbnode != nil)
    {
        postorder_node_walk(p_rbnode->left,nil);
        postorder_node_walk(p_rbnode->right,nil);
        printf("[%d]<->",p_rbnode->data);
    }
}

rbnode_t* get_rb_max_node(rbtree_t* p_tree, rbnode_t* p_rbnode)
{
    rbnode_t* p_run = p_rbnode;
    while(p_run->right != p_tree->nil)
        p_run = p_run->right;

    return (p_run);
}

rbnode_t* get_rb_min_node(rbtree_t* p_tree, rbnode_t* p_rbnode)
{
    rbnode_t* p_run = p_rbnode;
    while(p_run->left != p_tree->nil)
        p_run = p_run->left;

    return p_run;
}

rbnode_t* get_rbnode(data_t new_data, rbnode_t* nil)
{
    rbnode_t* p_new_node = NULL;

    p_new_node = (rbnode_t*)xcalloc(1,sizeof(rbnode_t));
    p_new_node->data = new_data;
    p_new_node->color = RED;
    p_new_node->left = nil;
    p_new_node->right = nil;
    p_new_node->parent = nil;

    return (p_new_node);
}

void *xcalloc(size_t nr_elements, size_t size_per_element)
{
    void* p = NULL;

    p = calloc(nr_elements, size_per_element);
    assert(p!= NULL);
    return (p);
}