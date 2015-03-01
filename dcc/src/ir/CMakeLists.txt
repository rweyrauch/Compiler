project(Decaf)

include_directories(.)

add_library(
    IrAst STATIC
    IrAssignExpr.cpp
    IrBinaryExpr.cpp
    IrBlock.cpp
    IrBooleanExpr.cpp
    IrBreakStmt.cpp
    IrClass.cpp
    IrCommon.cpp
    IrContinueStmt.cpp
    IrExprStmt.cpp
    IrFieldDecl.cpp
    IrForStmt.cpp
    IrIdentifier.cpp
    IrIfStmt.cpp
    IrLocation.cpp
    IrMethodCall.cpp
    IrMethodDecl.cpp
    IrReturnStmt.cpp
    IrStringLiteral.cpp
    IrSymbolTable.cpp
    IrTAC.cpp
    IrTravCtx.cpp
    IrVarDecl.cpp
)
    
