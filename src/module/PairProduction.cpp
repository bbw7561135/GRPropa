#include "grpropa/module/PairProduction.h"
#include "grpropa/Random.h"
#include "grpropa/Units.h"

#include <fstream>
#include <limits>
#include <stdexcept>

namespace grpropa {

PairProduction::PairProduction(PhotonField photonField, double thinning, double limit, double nMaxIterations) {
    setPhotonField(photonField);
    setThinning(thinning);
    setLimit(limit);
    setMaxNumberOfInteractions(nMaxIterations);
    initEnergyFractions();
}

void PairProduction::setPhotonField(PhotonField photonField) {
    this->photonField = photonField;
    switch (photonField) {
    case CMB:
        redshiftDependence = false;
        setDescription("Pair Production: CMB");
        initRate(getDataPath("PP-CMB.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-CMB.txt"));
        // std::cout << getDataPath("PP-CMB.txt") << std::endl;
        break;
    case EBL:  // default: Gilmore '12 IRB model
    case EBL_Gilmore12:
        redshiftDependence = true;
        setDescription("Pair  Production: EBL Gilmore et al. 2012");
        initRate(getDataPath("PP-EBL_Gilmore12.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Gilmore12.txt"));
        break;
    case EBL_Dominguez11:
        redshiftDependence = true;
        setDescription("Pair Production: EBL Dominguez et al. 2011");
        initRate(getDataPath("PP-EBL_Dominguez11.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Dominguez11.txt"));
        break;
    case EBL_Dominguez11_UL:
        redshiftDependence = true;
        setDescription("Pair Production: EBL Dominguez et al. 2011 (upper limit)");
        initRate(getDataPath("PP-EBL_Dominguez11_UL.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Dominguez11_UL.txt"));
        break;
    case EBL_Dominguez11_LL:
        redshiftDependence = true;
        setDescription("Pair Production: EBL Dominguez et al. 2011 (lower limit)");
        initRate(getDataPath("PP-EBL_Dominguez11_LL.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Dominguez11_LL.txt"));
        break;
    case EBL_Finke10:
        redshiftDependence = true;
        setDescription("Pair Production: EBL Finke et al. 2010");
        initRate(getDataPath("PP-EBL_Finke10.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Finke10.txt"));
        break;
    case EBL_Kneiske10:
        redshiftDependence = true;
        setDescription("Pair Production: EBL Kneiske & Dole 2010 (lower limit)");
        initRate(getDataPath("PP-EBL_Kneiske10.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Kneiske10.txt"));
        break;
    case EBL_Franceschini08:
        redshiftDependence = true;
        setDescription("Pair Production: EBL Franceschini et al. 2008");
        initRate(getDataPath("PP-EBL_Franceschini08.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-EBL_Franceschini08.txt"));
        break;
    case CRB:
    case CRB_Protheroe96:
        redshiftDependence = false;
        setDescription("Pair Production: CRB Protheroe & Biermann 1996");
        initRate(getDataPath("PP-CRB_Protheroe96.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-CRB_Protheroe96.txt"));
        break;
    case CRB_ARCADE2:
        redshiftDependence = false;
        setDescription("Pair Production: CRB ARCADE2 2010");
        initRate(getDataPath("PP-CRB_ARCADE2.txt"));
        initTableBackgroundEnergy(getDataPath("photonProbabilities-CRB_ARCADE2.txt"));
        break;
    default:
        throw std::runtime_error("PairProduction: unknown photon background");
    }
}

void PairProduction::setLimit(double limit) {
    this->limit = limit;
}

void PairProduction::setThinning(double a) {
    this->thinning = a;
}

void PairProduction::setMaxNumberOfInteractions(double a) {
    this->nMaxIterations = a;
}

void PairProduction::initRate(std::string filename) {
    
    if (redshiftDependence == false) {
        std::ifstream infile(filename.c_str());
        if (!infile.good())
            throw std::runtime_error("PairProduction: could not open file " + filename);
   
        // clear previously loaded interaction rates
        tabEnergy.clear();
        tabRate.clear();

        while (infile.good()) {
            if (infile.peek() != '#') {
                double a, b;
                infile >> a >> b;
                if (infile) {
                    tabEnergy.push_back(a * eV);
                    tabRate.push_back(b / Mpc);
                }
            }
            infile.ignore(std::numeric_limits < std::streamsize > ::max(), '\n');
        }
        infile.close();
    } else { // Rates for EBL
        std::ifstream infile(filename.c_str());
        if (!infile.good())
            throw std::runtime_error("PairProduction: could not open file " + filename);
  
        // clear previously loaded interaction rates
        tabEnergy.clear();
        tabRate.clear();
        tabRedshift.clear();

        // size of vector is predefined and depends on the model
        int nc; // number of columns (redshifts + one column for energy)
        int nl = 701; // number of lines (energies)
        std::vector<double> redshifts;
        if (photonField == EBL_Finke10) {
            nc = 33;
            double redshifts[] = {0.00, 0.01, 0.02, 0.03, 0.04, 0.05, 0.07, 0.09, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00, 1.20, 1.40, 1.60, 1.80, 2.00, 2.50, 3.00, 3.50, 4.00, 4.50, 4.99};
            for (int k=0; k<nc; k++) 
                tabRedshift.push_back(redshifts[k]);
        }
        else if (photonField == EBL_Gilmore12) {
            nc = 20;
            double redshifts[] = {0, 0.015, 0.025, 0.044, 0.05, 0.2, 0.4, 0.5, 0.6, 0.8, 1.0, 1.25, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0};
            for (int k=0; k<nc; k++) 
                tabRedshift.push_back(redshifts[k]);
        }
        else if (photonField == EBL_Dominguez11 || photonField == EBL_Dominguez11_UL || photonField == EBL_Dominguez11_LL) {
            nc = 18;
            double redshifts[] = {0, 0.01, 0.03, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0, 1.25, 1.5, 2.0, 2.5, 3.0, 3.9};
            for (int k=0; k<nc; k++) 
                tabRedshift.push_back(redshifts[k]);
        }
        else if (photonField == EBL_Kneiske10) {
            nc = 5;
            double redshifts[] = {0.0, 0.1, 0.3, 0.8, 2.0};
            for (int k=0; k<nc; k++) 
                tabRedshift.push_back(redshifts[k]);
        }
        else if (photonField == EBL_Franceschini08) {
            nc = 11;
            double redshifts[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0};
            for (int k=0; k<nc; k++) 
                tabRedshift.push_back(redshifts[k]);
        }
        else { 
            throw std::runtime_error("EBL model not defined for redshift dependent treatment (or not defined at all).");
        }

        double entries[nc+1][nl];
        int j = 0;
        while (!infile.eof()) {
            for (int i=0; i<nc+1; i++) {
                double entry = 0;
                infile >> entry;
                entries[i][j] = entry;
            }
            j++;
        }

        for (int j=0; j<nl; j++) 
            tabEnergy.push_back(entries[0][j] * eV);
        for (int i=1; i<nc+1; i++)
            for (int j=0; j<nl; j++)
                tabRate.push_back(entries[i][j] / Mpc);

        infile.close();

    } // conditional: redshift dependent
}

void PairProduction::initTableBackgroundEnergy(std::string filename) {

    if (redshiftDependence == false) {
        std::ifstream infile(filename.c_str());
        if (!infile.good())
            throw std::runtime_error("PairProduction: could not open file " + filename);
   
        // clear previously loaded interaction rates
        tabPhotonEnergy.clear();
        tabProb.clear();

        while (infile.good()) {
            if (infile.peek() != '#') {
                double a, b;
                infile >> b >> a;
                if (infile) {
                    tabPhotonEnergy.push_back(a * eV);
                    tabProb.push_back(b);
                }
            }
            infile.ignore(std::numeric_limits < std::streamsize > ::max(), '\n');
        }
        infile.close();
    } else { // Rates for EBL
        std::ifstream infile(filename.c_str());
        if (!infile.good())
            throw std::runtime_error("PairProduction: could not open file " + filename);
  
        // clear previously loaded interaction rates
        tabPhotonEnergy.clear();
        tabProb.clear();

        // size of vector is predefined and depends on the model
        int nc; // number of columns (redshifts + one column for energy)
        int nl = 701; // number of lines (probabilities)

        if (photonField == EBL_Finke10) nc = 33;
        else if (photonField == EBL_Gilmore12) nc = 20;
        else if (photonField == EBL_Kneiske10) nc = 5;
        else if (photonField == EBL_Franceschini08) nc = 11;
        else if (photonField == EBL_Dominguez11 || photonField == EBL_Dominguez11_UL || photonField == EBL_Dominguez11_LL) nc = 18;
        else throw std::runtime_error("EBL model not defined for redshift dependent treatment (or not defined at all).");

        double entries[nc+1][nl];
        int j = 0;
        while (!infile.eof()) {
            for (int i=0; i<nc+1; i++) {
                double entry = 0;
                infile >> entry;
                entries[i][j] = entry;
            }
            j++;
        }

        for (int j=0; j<nl; j++) 
            tabProb.push_back(entries[0][j]);
        for (int i=1; i<nc+1; i++)
            for (int j=0; j<nl; j++)
                tabPhotonEnergy.push_back(entries[i][j] * eV);

        infile.close();

    } // conditional: redshift dependent
}

void PairProduction::initEnergyFractions() {

    std::string filename = getDataPath("energyFraction-PP.txt");

    std::ifstream infile(filename.c_str());
    if (!infile.good())
        throw std::runtime_error("PairProduction: could not open file " + filename);

    // clear previously loaded interaction rates
    tabEnergyCM.clear();
    tabEnergyFractions.clear();
    tabEnergyFractionsProb.clear();

    int nc = 499; // number of columns (fractions + one column for s)
    int nl = 500; // number of lines (s)
    
    for (int i=1; i<=nc; i++){ // checked
        double p = (double) i / nc; 
        tabEnergyFractionsProb.push_back(p);
    }

    double entries[nc+1][nl];
    int j = 0;
    while (!infile.eof()) {
        for (int i=0; i<nc+1; i++) {
            double entry = 0;
            infile >> entry;
            entries[i][j] = entry;
        }
        j++;
    }


    for (int j=0; j<nl; j++) {
        tabEnergyCM.push_back(entries[0][j]);
    }
    int k = 0;
    for (int i=1; i<nc+1; i++) {
        for (int j=0; j<nl; j++){
            tabEnergyFractions.push_back(entries[i][j]);
        }
    }
    infile.close();
    // std::cout << "Pair Production: energy fractions loaded." << std::endl;
}

double PairProduction::energyFraction(double E, double z) const {
    /* 
        Returns the fraction of energy of the incoming photon taken by the
        outgoing electron.
        See ELMAG code for this implementation.
        Kachelriess et al., Comp. Phys. Comm 183 (2012) 1036

        Note: E is the correct value and should not be multiplied by (1+z).
              The redshift is used in this function only to draw the energy of the 
              background photon.
    */
    Random &random = Random::instance();
    double e;    
    if (redshiftDependence == true)
        e = interpolate2d(z, random.rand(), tabRedshift, tabProb, tabPhotonEnergy);
    else
        e = (1 + z) * interpolate(random.rand(), tabProb, tabPhotonEnergy);

    // kinematics
    double mu = random.randUniform(-1, 1);  
    double s = centerOfMassEnergy2(E, e, mu);
    double r = random.rand();
    for (int i=0; i<tabEnergyCM.size(); i++)
        Y.push_back(log10(tabEnergyCM[i]));

    double y = interpolate2d(s, r, tabEnergyCM, tabEnergyFractionsProb, tabEnergyFractions);
    // enforce charge conservation by rejecting all of such particles
    if (y == 0) y = -1;
    if (y == 1) y = -1;


    return y;
}

double PairProduction::centerOfMassEnergy2(double E, double e, double mu) const {
    return 2 * E * e * (1 - mu);
}

double PairProduction::lossLength(int id, double en, double z) const {
    
    if (id != 22)
        return std::numeric_limits<double>::max(); // no pair production by other particles

    if (en < tabEnergy.front())
        return std::numeric_limits<double>::max(); // below energy threshold

    double rate;
    if (redshiftDependence == false) {
        en *= (1 + z);
        if (en < tabEnergy.back())
            rate = interpolate(en, tabEnergy, tabRate); // interpolation
        else
            rate = tabRate.back() * pow(en / tabEnergy.back(), -0.6); // extrapolation
        rate *= pow(1 + z, 3);  
    } else {
        if (en < tabEnergy.back())
            rate = interpolate2d(z, en, tabRedshift, tabEnergy, tabRate); // interpolation
        else
            rate = tabRate.back() * pow(en / tabEnergy.back(), -0.6); // extrapolation
    }

    return 1. / rate;
}

void PairProduction::process(Candidate *c) const {

    // execute the loop at least once for limiting the next step
    double step = c->getCurrentStep();
    do {
        int id = c->current.getId();
        if (id != 22) 
            return; // only photons allowed

        double en = c->current.getEnergy();
        double z = c->getRedshift();
        double rate = 1 / lossLength(id, en, z);

        Random &random = Random::instance();
        double randDistance = -log(random.rand()) / rate;

        // check if an interaction occurs in this step
        if (step < randDistance) {
            // limit next step to a fraction of the mean free path
            c->limitNextStep(limit / rate);
            return;
        }
        performInteraction(c);

        // repeat with remaining steps
        step -= randDistance;
    } while (step > 0);
}

void PairProduction::performInteraction(Candidate *candidate) const {
    
    double en = candidate->current.getEnergy();
    double z = candidate->getRedshift();
    double f = energyFraction(en, z);

    Random &random = Random::instance();
      
    // // method 0  
    // if (random.rand() < pow(f, thinning) && f > 0 && f < 1) {
    //     double w0 = candidate->getWeight();
    //     double w = w0 / pow(f, thinning);
    //     candidate->addSecondary( 11, en * f, w);
    //     candidate->addSecondary(-11, en * (1 - f), w);
    //     candidate->setWeight(w);
    // } 

    // // method 1
    // if (f > 0 && f < 1){
    //     double w0 = candidate->getWeight();
    //     double wm = w0 / pow(f, thinning) / 2;
    //     double wp = w0 / pow(1 - f, thinning) / 2;
    //     if (r < 1 - pow(f, thinning) && r < 1 - pow(1 - f, thinning)){
    //         candidate->addSecondary(11, en * f, wp);  
    //         candidate->addSecondary(-11, en * (1 - f), wm);
    //     } else if (r < 1 - pow(f, thinning) && r > 1 - pow(1 - f, thinning)){
    //         candidate->addSecondary(11, en * f, wm);  
    //     } else if (r > 1 - pow(f, thinning) && r < pow(1 - f, thinning)){
    //         candidate->addSecondary(-11, en * (1 - f), wp);  
    //     } else { 
    //     }
    // }

    // method 2 - main (?)
    double w0 = candidate->getWeight();
    candidate->setActive(false);

    if (random.rand() < pow(f, thinning) && f > 0 && f < 1){
        double w = w0 / pow(f, thinning);
        candidate->addSecondary(11, en * f, w); 
        candidate->setWeight(w); 
    }

    if (random.rand() < pow(1 - f, thinning) && f > 0 && f < 1){
        double w = w0 / pow(1 - f, thinning);
        candidate->addSecondary(-11, en * (1 - f), w); 
        candidate->setWeight(w); 
    }

    // // original
    // candidate->setActive(false);
    // if (random.rand() < pow(f, thinning) && f > 0 && f < 1) {
    //     double w0 = candidate->getWeight();
    //     double w = w0 / pow(f, thinning);
    //     candidate->addSecondary( 11, en * f, w);
    //     candidate->addSecondary(-11, en * (1 - f), w);
    //     // candidate->setWeight(w);
    // }       
}

} // namespace grpropa
