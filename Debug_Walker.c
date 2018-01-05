/*
 * Debug_Node_Walker.c
 *
 *  Created on: Dec 25, 2017
 *      Author: Josip
 */

#include "System.h"
#include "Lexer.h"
#include "AST.h"
#include "Debug_Walker.h"

#define MAX_DBG_TREE_DEPTH  30

struct s_debug_walker_statistics
{
    uint8_t           Level;

    struct s_vertical_line
    {
        uint8_t Levels[MAX_DBG_TREE_DEPTH];
        uint8_t Idx;
    } VerLine;

    struct s_cur_node_info
    {
        e_ast_node_type   Type;
        void*             Ptr;
    } Current_Node;

} Debug_Walker_Statistics;

void Debug_Walker_Printout()
{
    static uint8_t LastLevel = 0;

    uint8_t Idx;
    uint8_t Idx_2;

    System_Print("\t"); /* Remove \t to remove margin */

    /* Check if a new line is needed */
    if(Debug_Walker_Statistics.Level < LastLevel)
    {
        for(Idx = 0; Idx < Debug_Walker_Statistics.Level-1; Idx++)
        {
            /* Check if a vertical line shall be drawn */
            for(Idx_2 = 0; Idx_2 < Debug_Walker_Statistics.VerLine.Idx; Idx_2++)
            {
                if(Debug_Walker_Statistics.VerLine.Levels[Idx_2] == Idx)
                {
                    /* Vertical line shall be drawn */
                    System_Print("|    ");
                    break;
                }
            }

            if(Idx_2 == Debug_Walker_Statistics.VerLine.Idx)
            {
                System_Print("     ");
            }
        }

        System_Print("|\n\t"); /* Remove \t to remove margin */
    }

    LastLevel = Debug_Walker_Statistics.Level;

    /* Draw current level */
    if(Debug_Walker_Statistics.Current_Node.Type != PROGRAM)
    {
        for(Idx = 0; Idx < Debug_Walker_Statistics.Level-1; Idx++)
        {
            /* Check if a vertical line shall be drawn */
            for(Idx_2 = 0; Idx_2 < Debug_Walker_Statistics.VerLine.Idx; Idx_2++)
            {
                if(Debug_Walker_Statistics.VerLine.Levels[Idx_2] == Idx)
                {
                    /* Vertical line shall be drawn */
                    System_Print("|    ");
                    break;
                }
            }

            if(Idx_2 == Debug_Walker_Statistics.VerLine.Idx)
            {
                System_Print("     ");
            }
        }

        System_Print("|____");
    }

    switch(Debug_Walker_Statistics.Current_Node.Type)
    {
    case PROGRAM:
        System_Print("PROGRAM\n");
        break;

    case VAR_DECL:
        System_Print("VAR_DECL\n");
        break;

    case TYPE:
        System_Print("TYPE -> %s\n", ((s_ast_type*)Debug_Walker_Statistics.Current_Node.Ptr)->token.value.string);
        break;

    case VAR:
        System_Print("VAR -> %s\n", ((s_ast_variable*)Debug_Walker_Statistics.Current_Node.Ptr)->token.value.string);
        break;

    case COMPOUND_MAIN:
        System_Print("COMPOUND_MAIN\n");
        break;

    case COMPOUND:
        System_Print("COMPOUND -> %s\n", ((s_ast_compound*)Debug_Walker_Statistics.Current_Node.Ptr)->name.value.string);
        break;

    case COMPOUND_CALL:
        System_Print("COMPOUND_CALL -> %s\n", ((s_ast_compoundStatementCall*)Debug_Walker_Statistics.Current_Node.Ptr)->cmpStatementName.value.string);
        break;

    case ARGUMENT:
        System_Print("ARGUMENT\n");
        break;

    case COMPOUND_RETURN:
        System_Print("COMPOUND_RETURN\n");
        break;

    case RETURN_STATEMENT:
        System_Print("RETURN\n");
        break;

    case FOR_STATEMENT:
        System_Print("FOR_STATEMENT\n");
        break;

    case BREAK_STATEMENT:
        System_Print("BREAK_STATEMENT\n");
        break;

    case IF_STATEMENT:
        System_Print("IF_STATEMENT\n");
        break;

    case IF_CONDITION:
        System_Print("IF_CONDITION\n");
        break;

    case ELSE_CONDITION:
        System_Print("ELSE_CONDITION\n");
        break;

    case PARAMETER:
        System_Print("PARAMETER\n");
        break;

    case ASSIGNMENT:
        System_Print("ASSIGNMENT\n");
        break;

    case BINARY_OP:
        System_Print("BINARY_OP -> %s\n", ((s_ast_binary*)Debug_Walker_Statistics.Current_Node.Ptr)->token.value.string);
        break;

    case UNARY_OP:
        System_Print("UNARY_OP -> %s\n", ((s_ast_unary*)Debug_Walker_Statistics.Current_Node.Ptr)->token.value.string);
        break;

    case NUM:
        System_Print("NUM -> %d\n", ((s_ast_num*)Debug_Walker_Statistics.Current_Node.Ptr)->token.value.integer_const);
        break;

    case NO_OP:
        /* Not Implemented Yet */
        break;
    }
}

void Debug_Walker_Visit( void* NodePtr )
{
    void* TempNodePtr;

    /* Increment Level */
    Debug_Walker_Statistics.Level++;

    /* Update Statistics */
    Debug_Walker_Statistics.Current_Node.Type = ((s_ast_node_info*)NodePtr)->type;
    Debug_Walker_Statistics.Current_Node.Ptr = NodePtr;

    /* Determine Node Type */
    switch( ((s_ast_node_info*)NodePtr)->type )
    {
    case PROGRAM:
        /* Debug Printout */
        Debug_Walker_Printout( PROGRAM );

        /* Add Vertical Line */
        if(((s_ast_program*)NodePtr)->global_var_decl_link.var_declaration != NULL ||
           ((s_ast_program*)NodePtr)->compound_link.compound_statement != NULL     )
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Global Variable Declaration Visit */
        TempNodePtr = (void*)&((s_ast_program*)NodePtr)->global_var_decl_link;
        while( TempNodePtr != NULL && ((struct global_var_decl_link*)TempNodePtr)->var_declaration != NULL )
        {
            Debug_Walker_Visit( ((struct global_var_decl_link*)TempNodePtr)->var_declaration );

            TempNodePtr = (void*)((struct global_var_decl_link*)TempNodePtr)->next_var_decl_link;
        }

        /* Compound Statement Visit */
        TempNodePtr = (void*)&((s_ast_program*)NodePtr)->compound_link;
        while( TempNodePtr != NULL && ((struct compound_link*)TempNodePtr)->compound_statement != NULL )
        {
            Debug_Walker_Visit( ((struct compound_link*)TempNodePtr)->compound_statement );

            TempNodePtr = (void*)((struct compound_link*)TempNodePtr)->next_compound_link;
        }

        /* Remove Vertical Line */
        Debug_Walker_Statistics.VerLine.Idx--;

        /* Visit Compound Main */
        Debug_Walker_Visit( ((s_ast_program*)NodePtr)->compound_main );
        break;

    case VAR_DECL:
        /* Debug Printout */
        Debug_Walker_Printout( VAR_DECL );

        /* Type Node Visit */
        Debug_Walker_Visit( ((s_ast_var_declaration*)NodePtr)->type );

        /* Variable Node Visits */
        TempNodePtr = (void*)&((s_ast_var_declaration*)NodePtr)->var_link;
        while( TempNodePtr != NULL && ((struct var_link*)TempNodePtr)->variable != NULL )
        {
            Debug_Walker_Visit( ((struct var_link*)TempNodePtr)->variable );

            TempNodePtr = (void*)((struct var_link*)TempNodePtr)->next_var_link;
        }
        break;

    case COMPOUND_MAIN:
        /* Debug Printout */
        Debug_Walker_Printout( COMPOUND_MAIN );

        /* Add Vertical Line */
        if(((s_ast_compound_main*)NodePtr)->main_var_decl_link.var_declaration != NULL ||
           ((s_ast_compound_main*)NodePtr)->statement_link.statement != NULL     )
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Main Variable Declaration Visit */
        TempNodePtr = (void*)&((s_ast_compound_main*)NodePtr)->main_var_decl_link;
        while( TempNodePtr != NULL && ((struct main_var_decl_link*)TempNodePtr)->var_declaration != NULL )
        {
            Debug_Walker_Visit( ((struct main_var_decl_link*)TempNodePtr)->var_declaration );

            TempNodePtr = (void*)((struct main_var_decl_link*)TempNodePtr)->next_var_decl_link;
        }

        /* Statement Node Visit */
        TempNodePtr = (void*)&((s_ast_compound_main*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            /* Check if this is the last statement */
            if(((struct s_ast_statement_link*)TempNodePtr)->next_statement_link == NULL)
            {
                /* Remove Vertical Line */
                Debug_Walker_Statistics.VerLine.Idx--;
            }

            Debug_Walker_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case COMPOUND:
        /* Debug Printout */
        Debug_Walker_Printout( COMPOUND );

        /* Add Vertical Line */
        if(((s_ast_compound*)NodePtr)->parameter_link.parameter != NULL ||
           ((s_ast_compound*)NodePtr)->cmp_var_decl_link.var_declaration != NULL ||
           ((s_ast_compound*)NodePtr)->statement_link.statement != NULL)
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Compound Return Type Visit */
        Debug_Walker_Visit( ((s_ast_compound*)NodePtr)->return_type );

        /* Parameter Visit */
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->parameter_link;
        while( TempNodePtr != NULL && ((struct parameter_link*)TempNodePtr)->parameter != NULL )
        {
            Debug_Walker_Visit( ((struct parameter_link*)TempNodePtr)->parameter );

            TempNodePtr = (void*)((struct parameter_link*)TempNodePtr)->next_parameter_link;
        }

        /* Compound Variable Declaration Visit */
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->cmp_var_decl_link;
        while( TempNodePtr != NULL && ((struct cmp_var_decl_link*)TempNodePtr)->var_declaration != NULL )
        {
            Debug_Walker_Visit( ((struct cmp_var_decl_link*)TempNodePtr)->var_declaration );

            TempNodePtr = (void*)((struct cmp_var_decl_link*)TempNodePtr)->next_var_decl_link;
        }

        /* Statement Node Visit */
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            /* Check if this is the last statement */
            if(((struct s_ast_statement_link*)TempNodePtr)->next_statement_link == NULL)
            {
                /* Remove Vertical Line */
                Debug_Walker_Statistics.VerLine.Idx--;
            }

            Debug_Walker_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case COMPOUND_CALL:
        /* Debug Printout */
        Debug_Walker_Printout( COMPOUND_CALL );

        /* Add Vertical Line */
        if(((s_ast_compoundStatementCall*)NodePtr)->argument_link.argument != NULL)
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Compound Call Argument Visit */
        TempNodePtr = (void*)&((s_ast_compoundStatementCall*)NodePtr)->argument_link;
        while( TempNodePtr != NULL && ((struct argument_link*)TempNodePtr)->argument != NULL )
        {
            /* Check if this is the last argument */
            if(((struct argument_link*)TempNodePtr)->next_argument_link == NULL)
            {
                /* Remove Vertical Line */
                Debug_Walker_Statistics.VerLine.Idx--;
            }

            Debug_Walker_Visit( ((struct argument_link*)TempNodePtr)->argument );

            TempNodePtr = (void*)((struct argument_link*)TempNodePtr)->next_argument_link;
        }
        break;

    case ARGUMENT:
        /* Debug Printout */
        Debug_Walker_Printout( ARGUMENT );

        /* Visit Argument Nodes */
        Debug_Walker_Visit( ((s_ast_compoundCallArg*)NodePtr)->argument );
        break;

    case COMPOUND_RETURN:
        /* Debug Printout */
        Debug_Walker_Printout( COMPOUND_RETURN );

        /* Type Visit */
        Debug_Walker_Visit( ((s_ast_compound_return*)NodePtr)->type );
        break;

    case RETURN_STATEMENT:
        /* Debug Printout */
        Debug_Walker_Printout( RETURN );

        /* Visit Return Value */
        Debug_Walker_Visit( ((s_ast_compoundReturnStatement*)NodePtr)->value );
        break;

    case FOR_STATEMENT:
        /* Debug Printout */
        Debug_Walker_Printout( FOR_STATEMENT );

        /* Add Vertical Line */
        Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
        Debug_Walker_Statistics.VerLine.Idx++;

        /* Visit Init Statement */
        if( ((s_ast_forStatement*)NodePtr)->initStatement != NULL )
        {
            Debug_Walker_Visit( ((s_ast_forStatement*)NodePtr)->initStatement );
        }

        /* Visit Post Statement */
        if( ((s_ast_forStatement*)NodePtr)->postStatement != NULL )
        {
            Debug_Walker_Visit( ((s_ast_forStatement*)NodePtr)->postStatement );
        }

        /* Visit Condition Statement */
        if( ((s_ast_forStatement*)NodePtr)->condition != NULL )
        {
            Debug_Walker_Visit( ((s_ast_forStatement*)NodePtr)->condition );
        }

        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_forStatement*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            /* Check if this is the last argument */
            if(((struct s_ast_statement_link*)TempNodePtr)->next_statement_link == NULL)
            {
                /* Remove Vertical Line */
                Debug_Walker_Statistics.VerLine.Idx--;
            }

            Debug_Walker_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case BREAK_STATEMENT:
        /* Debug Printout */
        Debug_Walker_Printout( BREAK_STATEMENT );
        break;

    case IF_STATEMENT:
        /* Debug Printout */
        Debug_Walker_Printout( IF_STATEMENT );

        /* Add Vertical Line */
        if(((s_ast_ifStatement*)NodePtr)->if_condition_link.ifCondition != NULL)
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Visit If Conditions */
        TempNodePtr = (void*)&((s_ast_ifStatement*)NodePtr)->if_condition_link;
        while( TempNodePtr != NULL && ((struct if_condition_link*)TempNodePtr)->ifCondition != NULL )
        {
            if(((s_ast_ifStatement*)NodePtr)->elseCondition == NULL)
            {
                /* Check if this is the last argument */
                if(((struct if_condition_link*)TempNodePtr)->next_ifCondition_link == NULL)
                {
                    /* Remove Vertical Line */
                    Debug_Walker_Statistics.VerLine.Idx--;
                }
            }

            Debug_Walker_Visit( ((struct if_condition_link*)TempNodePtr)->ifCondition );

            TempNodePtr = (void*)((struct if_condition_link*)TempNodePtr)->next_ifCondition_link;
        }

        /* Visit Else Condition */
        if(((s_ast_ifStatement*)NodePtr)->elseCondition != NULL)
        {
            /* Remove Vertical Line */
            Debug_Walker_Statistics.VerLine.Idx--;

            Debug_Walker_Visit( ((s_ast_ifStatement*)NodePtr)->elseCondition );
        }
        break;

    case IF_CONDITION:
        /* Debug Printout */
        Debug_Walker_Printout( IF_CONDITION );

        /* Add Vertical Line */
        if(((s_ast_ifCondition*)NodePtr)->statement_link.statement != NULL)
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Visit Condition */
        Debug_Walker_Visit( ((s_ast_ifCondition*)NodePtr)->condition );

        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_ifCondition*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            /* Check if this is the last argument */
            if(((struct s_ast_statement_link*)TempNodePtr)->next_statement_link == NULL)
            {
                /* Remove Vertical Line */
                Debug_Walker_Statistics.VerLine.Idx--;
            }

            Debug_Walker_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case ELSE_CONDITION:
        /* Debug Printout */
        Debug_Walker_Printout( ELSE_CONDITION );

        /* Add Vertical Line */
        if(((s_ast_elseCodnition*)NodePtr)->statement_link.statement != NULL)
        {
            Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
            Debug_Walker_Statistics.VerLine.Idx++;
        }

        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_elseCodnition*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            /* Check if this is the last argument */
            if(((struct s_ast_statement_link*)TempNodePtr)->next_statement_link == NULL)
            {
                /* Remove Vertical Line */
                Debug_Walker_Statistics.VerLine.Idx--;
            }

            Debug_Walker_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case PARAMETER:
        /* Debug Printout */
        Debug_Walker_Printout( PARAMETER );

        /* Type Visit */
        Debug_Walker_Visit( ((s_ast_parameter*)NodePtr)->type );

        /* Variable Visit */
        Debug_Walker_Visit( ((s_ast_parameter*)NodePtr)->variable );
        break;

    case ASSIGNMENT:
        /* Debug Printout */
        Debug_Walker_Printout( ASSIGNMENT );

        /* Variable (Left Operand) Visit */
        Debug_Walker_Visit( ((s_ast_assignment*)NodePtr)->variable );

        /* Expression Visit */
        Debug_Walker_Visit( ((s_ast_assignment*)NodePtr)->expression );
        break;

    case BINARY_OP:
        /* Debug Printout */
        Debug_Walker_Printout( BINARY_OP );

        /* Add Vertical Line */
        Debug_Walker_Statistics.VerLine.Levels[Debug_Walker_Statistics.VerLine.Idx] = Debug_Walker_Statistics.Level;
        Debug_Walker_Statistics.VerLine.Idx++;

        /* Visit Left Operand */
        Debug_Walker_Visit( ((s_ast_binary*)NodePtr)->leftOperand );

        /* Remove Vertical Line */
        Debug_Walker_Statistics.VerLine.Idx--;

        /* Visit Right Operand */
        Debug_Walker_Visit( ((s_ast_binary*)NodePtr)->rightOperand );
        break;

    case UNARY_OP:
        /* Debug Printout */
        Debug_Walker_Printout( UNARY_OP );

        /* Visit Operand */
        Debug_Walker_Visit( ((s_ast_unary*)NodePtr)->operand );
        break;

    case NUM:
        /* Debug Printout */
        Debug_Walker_Printout( UNARY_OP );
        break;

    case TYPE:
        /* Debug Printout */
        Debug_Walker_Printout( TYPE );
        break;

    case VAR:
        /* Debug Printout */
        Debug_Walker_Printout( VAR );
        break;

    default:
        printf("Node Type Unknown: %d\n", ((s_ast_node_info*)NodePtr)->type);
        exit(1);
        break;
    }

    /* Decrement Level */
    Debug_Walker_Statistics.Level--;
}

void Debug_Walker_Walkthrought( s_ast_program* ProgramNode )
{
    /* Init Statistics */
    Debug_Walker_Statistics.Level = 255;  /* Will overflow to 0 on first visit */
    Debug_Walker_Statistics.Current_Node.Type = PROGRAM;

    System_Print("Abstract Syntax Tree:\n \n");

    Debug_Walker_Visit( ProgramNode );
}
