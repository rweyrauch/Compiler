project(Decaf)

include_directories(.)

add_library(
    IrAst STATIC
    IrAssignExpr.cpp
    IrBasicBlock.cpp
    IrBinaryExpr.cpp
    IrBlock.cpp
    IrBooleanExpr.cpp
    IrBreakStmt.cpp
    IrClass.cpp
    IrCommon.cpp
    IrContinueStmt.cpp
    IrDoWhileStmt.cpp
    IrExprStmt.cpp
    IrFieldDecl.cpp
    IrForStmt.cpp
    IrGotoStmt.cpp
    IrIdentifier.cpp
    IrIfStmt.cpp
    IrInterface.cpp
    IrLabelStmt.cpp
    IrLocation.cpp
    IrMethodCall.cpp
    IrMethodDecl.cpp
    IrOptimizer.cpp
    IrProgram.cpp
    IrReturnStmt.cpp
    IrStringLiteral.cpp
    IrSymbolTable.cpp
    IrTAC.cpp
    IrTravCtx.cpp
    IrVarDecl.cpp
    IrWhileStmt.cpp
)
    
