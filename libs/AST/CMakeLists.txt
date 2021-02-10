add_library(ast STATIC ASTNode.cpp ASTStatementNode.cpp ASTExprNode.cpp)

target_link_libraries(ast PUBLIC symbol)
