#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <string>

#include "hindleymilner.hpp"
#include "type_checker.hpp"

namespace HindleyMilner {

// the monotypes' id shouldn't start before the native
// types id
int id = NATIVE_TYPES;

// map from id to monotype
std::unordered_map<int, Mono*> mono_id{
    {0, new Mono{0}},
    {1, new Mono{1}},               // fixed values for representative
    {2, new Mono{2}},               // of native types
    {3, new Mono{3}},
    {4, new Mono{4}},
    {5, new Mono{5}},
};

Mono* new_mono () {
    mono_id[id] = new Mono {id};
    return mono_id[id++];
}

Mono* new_param (const Mono base) {
    mono_id[id] = new Param {id, &base};
    return mono_id[id++];
}

Mono* new_instance (const Mono base) {
    assert(mono_id.count(base.id));
    return mono_id[base.id];
}

Mono* instanciate(Mono* base, std::unordered_map<int, Mono*>& forall) {
    // this function should recursionate over the tree type
    // changing the quantified types in the forall specifier
    // for new fresh types
    
    // TODO check if its fixed
    // si hay un tipo a->a
    // el algoritmo actual lo instancia b->c
    // ademas el algoritmo usa el id del tipo que quizas no 
    // es su representacion actual (representative)
    // entonces a->a podria verlo como a->b
    if (base->type == mono_type::Mono) {
        Mono* rep = representative(base);
        if (forall.count(rep->id)) {
            if (forall[rep->id]) {
                return forall[rep->id];
            } else {
                forall[rep->id] = new Mono {mono_type::Mono, id};
                mono_id[id] = forall[rep->id];
            }
        } else {
            return base;
        }
    } else {
        auto p_base = static_cast<Param*>(base);

        mono_id[id] = new Param {id, p_base->base};

        std::vector<Mono*> fresh_params;
        for (auto param : p_base->params) {
            fresh_params.push_back(instanciate(param, forall));
        }
        
        auto param = static_cast<Param*>(mono_id[id]);
        param->params = fresh_params;
    }

    return mono_id[id++];
}

Mono* instanciate(Poly type) {
    std::unordered_map<int, Mono*> forall;
    for (int id : type.forall_ids) {
        forall[id] = nullptr;
    }
    return instanciate(type.base, forall);
}

Mono* hm_var (Poly type) {
    mono_id[id] = instanciate(type);
    
    return mono_id[id++]; 
}

Mono* representative(Mono* t) {
    Mono* rep = mono_id[t->id];
    if (rep->id == t->id) {
        return rep;
    } else {
        Mono* rep_rep = representative(rep);
        t->id = rep_rep->id;
        return rep_rep;
    }
}

bool appears_in(Mono* var, Mono* type) {
    // checks whether var appears in type

    if (type->type == mono_type::Mono) {
        return (var->id == type->id);
    } else {
        Param* p_type = static_cast<Param*>(type);

        for (Mono* param : p_type->params) {
            if (appears_in(var, param)) {
                return true;
            }
        }
    }

    return false;
}

void connect(Mono* r1, Mono* r2) {
    // connect expects two representatives
    // and the first has to be a variable
    assert(r1->type == mono_type::Mono);

    // the function should check r1 doesn't appear in r2
    if (r2->type == mono_type::Param) {
        Param* p2 = static_cast<Param*>(r2);

        for (Mono* param : p2->params) {
            assert(!appears_in(r1, param));
        }
    }

    r1->id = r2->id;
}

void unify(Mono* t1, Mono* t2, Env env) {
    Mono* r1 = representative(t1);
    Mono* r2 = representative(t2);

    // checkear de no conectar una variable con
    // un tipo que contenga la variable
    // Ej: f(f) = unify(a->b, (a->b)->c)
    //  o  x(x) = unify(a, a->b)
    //  o  f(list f) = unify(a->b, (list (a->b))->c)
    if (t1->type == mono_type::Mono) {
        if (t2->type == mono_type::Mono) {
            if (!env.is_bound(r1))
                connect(r1, r2);
            else if (!env.is_bound(r2))
                connect(r2, r1);
            else
                assert(r1 == r2);
        } else {
            assert(!env.is_bound(r1));
            connect(r1, r2);
        }
    } else if (t2->type == mono_type::Mono) {
        assert(!env.is_bound(r2));
        connect(r2, r1);
    } else {
        Param* p1 = static_cast<Param*>(t1);
        Param* p2 = static_cast<Param*>(t2);

        assert(p1->params.size() == p2->params.size());
        assert(p1->base == p2->base);

        for (int i = 0; i < (int)p1->params.size(); i++) {
            unify(p1->params[i], p2->params[i], env);
        }
    }
}

Mono* hm_app (Mono* t1, Mono* t2, Env env) {
    Mono* t3 = new_mono();

    mono_id[id] = new Param {id, &Arrow};

    Param* param = static_cast<Param*>(mono_id[id]);
    param->params = std::vector<Mono*>{t2, t3};

    unify(t1, mono_id[id++], env);

    return t3;
}

Mono* hm_abs (Mono* args, Mono* ret) { // ret(urn)
    mono_id[id] = new Param {id, &Arrow};

    Param* param = static_cast<Param*>(mono_id[id]);
    param->params = std::vector<Mono*>{args, ret};

    return mono_id[id++];
}

Poly hm_let (Poly value, std::vector<int> free_vars) {
    // las variables libres deben asignarse al
    // tipo poly en los valores forall

    for (auto id : free_vars) {
        value.forall_ids.push_back(id);
    }

    return value;
}

}