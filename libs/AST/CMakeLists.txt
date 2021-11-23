add_library(ast STATIC ASTNode.cpp ASTStatementNode.cpp ASTExprNode.cpp ASTMethodExtractor.cpp)

target_link_libraries(ast PUBLIC symbol)
