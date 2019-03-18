#ifndef TUCUXI_CORE_INVARIANTS_H
#define TUCUXI_CORE_INVARIANTS_H


namespace Tucuxi {
namespace Core {


enum class Invariants {
    INV_0000 = 0,
    INV_ANALYTESET_0001,
    INV_ANALYTESET_0002,
    INV_ANALYTESET_0003,
    INV_ANALYTESET_0004,
    INV_ANALYTESET_0005,
    INV_ANALYTESET_0006,
    INV_ANALYTE_0001,
    INV_ANALYTE_0002,
    INV_ANALYTE_0003,
    INV_CONSTRAINT_0001,
    INV_CONSTRAINT_0002,
    INV_CONSTRAINT_0003,
    INV_COVARIATEDEFINITION_0001,
    INV_DRUGMODEL_0001,
    INV_DRUGMODEL_0002,
    INV_DRUGMODEL_0003,
    INV_DRUGMODEL_0004,
    INV_DRUGMODEL_0005,
    INV_DRUGMODEL_0006,
    INV_DRUGMODEL_0007,
    INV_DRUGMODEL_0008,
    INV_DRUGMODEL_0009,
    INV_DRUGMODEL_0010,
    INV_DRUGMODEL_0011,
    INV_DRUGMODEL_0012,
    INV_DRUGMODEL_0013,
    INV_DRUGMODEL_0014,
    INV_DRUGMODELDOMAIN_0001,
    INV_FORMULATIONANDROUTE_0001,
    INV_FORMULATIONANDROUTE_0002,
    INV_FORMULATIONANDROUTE_0003,
    INV_FULLFORMULATIONANDROUTE_0001,
    INV_FULLFORMULATIONANDROUTE_0002,
    INV_FULLFORMULATIONANDROUTE_0003,
    INV_FULLFORMULATIONANDROUTE_0004,
    INV_FULLFORMULATIONANDROUTE_0005,
    INV_FULLFORMULATIONANDROUTE_0006,
    INV_FULLFORMULATIONANDROUTE_0007,
    INV_FULLFORMULATIONANDROUTE_0008,
    INV_FULLFORMULATIONANDROUTE_0009,
    INV_HALFLIFE_0001,
    INV_HALFLIFE_0002,
    INV_TIMECONSIDERATIONS_0001,
    INV_TIMECONSIDERATIONS_0002,
    INV_TIMECONSIDERATIONS_0003,
    INV_TIMECONSIDERATIONS_0004
};

// TODO : Un DrugModelChecker

// TODO : Afficher le fichier et ligne
#define INVARIANT(invariant, expression) { \
    bool result = expression; if (!result) { \
    std::cout << "Invariant failure : " << \
    static_cast<int>(invariant) << \
    ". File " << __FILE__ << ", line " << __LINE__ << \
    std::endl;} \
    ok &= result; \
    if (!ok) {return false;} }

#define COMPLEX_INVARIANT(invariant, expression) expression;
#define INVARIANTS(decl) public : bool checkInvariants() const {bool ok=true;decl;return ok;}
#define CHECKINVARIANTS checkInvariants()
#define LAMBDA_INVARIANT(invariant, expression) {auto f=[this]() {expression}; {bool result = f();if (!result) {std::cout << "Invariant failure : " << static_cast<int>(invariant) << std::endl;} ok &= result; if (!ok) return false;}}

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_INVARIANTS_H
