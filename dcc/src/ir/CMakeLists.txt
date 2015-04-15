project(Decaf)

include_directories(.)

add_library(
    IrAst STATIC
    IrAssignExpr.cpp
    IrBasicBlock.cpp
    IrBinaryExpr.cpp
    IrBlock.cpp
    IrBooleanExpr.cpp
    IrBoolLiteral.cpp
    IrBreakStmt.cpp
    IrCaseStmt.cpp
    IrClass.cpp
    IrCommon.cpp
    IrContinueStmt.cpp
    IrDoWhileStmt.cpp
    IrDoubleLiteral.cpp
    IrExprStmt.cpp
    IrFieldDecl.cpp
    IrForStmt.cpp
    IrGotoStmt.cpp
    IrIdentifier.cpp
    IrIfStmt.cpp
    IrInterface.cpp
    IrIntLiteral.cpp
    IrLabelStmt.cpp
    IrLocation.cpp
    IrMethodCall.cpp
    IrMethodDecl.cpp
    IrOptimizer.cpp
    IrProgram.cpp
    IrReturnStmt.cpp
    IrStringLiteral.cpp
    IrSwitchStmt.cpp
    IrSymbolTable.cpp
    IrTAC.cpp
    IrTravCtx.cpp
    IrVarDecl.cpp
    IrWhileStmt.cpp
)
    
