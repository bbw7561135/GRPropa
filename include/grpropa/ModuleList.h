#ifndef GRPROPA_MODULE_LIST_H
#define GRPROPA_MODULE_LIST_H

#include "grpropa/Candidate.h"
#include "grpropa/Module.h"
#include "grpropa/Source.h"

#include <list>
#include <sstream>

namespace grpropa {


/**
 @class ModuleList
 @brief The simulation itself: A list of simulation modules
 */
class ModuleList: public Module {
public:
    typedef std::list<ref_ptr<Module> > module_list_t;
    typedef std::vector<ref_ptr<Candidate> > candidate_vector_t;

    ModuleList();
    virtual ~ModuleList();
    void setShowProgress(bool show = true); ///< activate a progress bar

    void add(Module* module);
    module_list_t &getModules();
    const module_list_t &getModules() const;

    void process(Candidate *candidate) const; ///< call process in all modules
    // void processToFinish(Candidate *candidate, bool recursive = true); ///< propagate until finished

    void run(Candidate *candidate, bool recursive = true); ///< run simulation for a single candidate
    void run(candidate_vector_t &candidates, bool recursive = true); ///< run simulation for a candidate vector
    void run(SourceInterface *source, size_t count, bool recursive = true); ///< run simulation for n candidates from the given source

    std::string getDescription() const;
    void showModules() const;

private:
    module_list_t modules;
    bool showProgress;
};

/**
 @class ModuleListRunner
 @brief Run the provided ModuleList when process is called.
 */
class ModuleListRunner: public Module {
private:
    ref_ptr<ModuleList> mlist;
public:

    ModuleListRunner(ModuleList *mlist);
    void process(Candidate *candidate) const; ///< call run of wrapped ModuleList
    std::string getDescription() const;
};

} // namespace grpropa

#endif // GRPROPA_MODULE_LIST_H
