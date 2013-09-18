// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/exteriorpower.hpp by Victor Dods, created 2013/08/15
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_EXTERIORPOWER_HPP_
#define TENH_CONCEPTUAL_EXTERIORPOWER_HPP_

#include "tenh/core.hpp"

#include "tenh/meta/typelist_utility.hpp"
#include "tenh/conceptual/concept.hpp"
#include "tenh/conceptual/embeddableintensorproduct.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/conceptual/vectorspace.hpp"
#include "tenh/mathutil.hpp"

namespace Tenh {

// exterior power of formal symbols (e.g. identifiers, builtin C++ types, etc)
template <Uint32 ORDER_, typename Factor_>
struct ExteriorPower_c
{
    typedef EmptyTypeList ParentTypeList;

    static Uint32 const ORDER = ORDER_;
    typedef Factor_ Factor;
    typedef typename TypeListWithMultiplicity_t<Factor_,ORDER_>::T FactorTypeList;

    static std::string type_as_string ()
    {
        return "ExteriorPower_c<" + AS_STRING(ORDER_) + ',' + TypeStringOf_t<Factor_>::eval() + '>';
    }
};

template <Uint32 ORDER_, typename Factor_>
struct IsConcept_f<ExteriorPower_c<ORDER_,Factor_> >
{ static bool const V = true; };

template <typename T> struct IsExteriorPower_f { static bool const V = false; };
template <Uint32 ORDER, typename Factor> struct IsExteriorPower_f<ExteriorPower_c<ORDER,Factor> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(ExteriorPower);
// special convenience macros
#define IS_EXTERIOR_POWER_UNIQUELY(Concept) HasUniqueExteriorPowerStructure_f<Concept>::V
#define AS_EXTERIOR_POWER(Concept) UniqueExteriorPowerStructureOf_f<Concept>::T


template <Uint32 ORDER_, typename Factor_>
struct ExteriorPowerOfVectorSpace_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_VECTOR_SPACE), };

    typedef ExteriorPower_c<ORDER_,Factor_> As_ExteriorPower;
    typedef VectorSpace_c<typename Factor_::Field,BinomialCoefficient_t<AS_VECTOR_SPACE(Factor_)::DIMENSION, ORDER_>::V,ExteriorPower_c<ORDER_,typename Factor_::Id> > As_VectorSpace;
    typedef EmbeddableInTensorProductOfVectorSpaces_c<typename TensorPowerOfVectorSpace_f<ORDER_,Factor_>::T> As_EmbeddableInTensorProductOfVectorSpaces;
public:
    typedef TypeList_t<As_ExteriorPower,
            TypeList_t<As_VectorSpace,
            TypeList_t<As_EmbeddableInTensorProductOfVectorSpaces> > > ParentTypeList;

    typedef typename As_VectorSpace::Id Id;
    typedef Factor_ Factor;

    static std::string type_as_string ()
    {
        return "ExteriorPowerOfVectorSpace_c<" + AS_STRING(ORDER_) + ',' + TypeStringOf_t<Factor_>::eval() + '>';
    }
};

template <Uint32 ORDER_, typename Factor_>
struct IsConcept_f<ExteriorPowerOfVectorSpace_c<ORDER_,Factor_> >
{ static bool const V = true; };

template <typename T> struct IsExteriorPowerOfVectorSpace_f { static bool const V = false; };
template <Uint32 ORDER, typename Factor> struct IsExteriorPowerOfVectorSpace_f<ExteriorPowerOfVectorSpace_c<ORDER,Factor> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(ExteriorPowerOfVectorSpace);
// special convenience macros
#define IS_EXTERIOR_POWER_OF_VECTOR_SPACE_UNIQUELY(Concept) HasUniqueExteriorPowerOfVectorSpaceStructure_f<Concept>::V
#define AS_EXTERIOR_POWER_OF_VECTOR_SPACE(Concept) UniqueExteriorPowerOfVectorSpaceStructureOf_f<Concept>::T


// Factor_ must be a Basis_c type
template <Uint32 ORDER_, typename Factor_>
struct ExteriorPowerOfBasis_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_BASIS_UNIQUELY(Factor_), MUST_BE_BASIS) };
    typedef ExteriorPower_c<ORDER_,Factor_> As_ExteriorPower;
    typedef Basis_c<ExteriorPower_c<ORDER_,Factor_> > As_Basis;
public:
    typedef TypeList_t<As_ExteriorPower,
            TypeList_t<As_Basis> > ParentTypeList;

    typedef typename As_Basis::Id Id;
    typedef Factor_ Factor;

    static std::string type_as_string ()
    {
        return "ExteriorPowerOfBasis_c<" + AS_STRING(ORDER_) + ',' + TypeStringOf_t<Factor_>::eval() + '>';
    }
};

template <Uint32 ORDER_, typename Factor_>
struct IsConcept_f<ExteriorPowerOfBasis_c<ORDER_,Factor_> >
{ static bool const V = true; };

template <typename T> struct IsExteriorPowerOfBasis_f { static bool const V = false; };
template <Uint32 ORDER, typename Factor> struct IsExteriorPowerOfBasis_f<ExteriorPowerOfBasis_c<ORDER,Factor> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(ExteriorPowerOfBasis);
// special convenience macros
#define IS_EXTERIOR_POWER_OF_BASES_UNIQUELY(Concept) HasUniqueExteriorPowerOfBasisStructure_f<Concept>::V
#define AS_EXTERIOR_POWER_OF_BASES(Concept) UniqueExteriorPowerOfBasisStructureOf_f<Concept>::T


template <typename ExteriorPowerOfVectorSpace_, typename Basis_>
struct BasedExteriorPowerOfVectorSpace_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IS_EXTERIOR_POWER_OF_VECTOR_SPACE_UNIQUELY(ExteriorPowerOfVectorSpace_), MUST_BE_EXTERIOR_POWER_OF_VECTOR_SPACE),
        STATIC_ASSERT_IN_ENUM(IS_BASIS_UNIQUELY(Basis_), MUST_BE_BASIS),
    };
    typedef ExteriorPowerOfVectorSpace_ As_ExteriorPowerOfVectorSpace;
    typedef BasedVectorSpace_c<ExteriorPowerOfVectorSpace_,Basis_> As_BasedVectorSpace;
public:
    typedef TypeList_t<As_ExteriorPowerOfVectorSpace,
            TypeList_t<As_BasedVectorSpace> > ParentTypeList;

    typedef typename As_BasedVectorSpace::Id Id;

    static std::string type_as_string ()
    {
        return "BasedExteriorPowerOfVectorSpace_c<" + TypeStringOf_t<ExteriorPowerOfVectorSpace_>::eval() + ','
                                                    + TypeStringOf_t<Basis_>::eval() + '>';
    }
};

template <typename ExteriorPowerOfVectorSpace, typename Basis_>
struct IsConcept_f<BasedExteriorPowerOfVectorSpace_c<ExteriorPowerOfVectorSpace, Basis_> >
{ static bool const V = true; };

template <typename T> struct IsBasedExteriorPowerOfVectorSpace_f { static bool const V = false; };
template <typename ExteriorPowerOfVectorSpace, typename Basis> struct IsBasedExteriorPowerOfVectorSpace_f<BasedExteriorPowerOfVectorSpace_c<ExteriorPowerOfVectorSpace,Basis> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(BasedExteriorPowerOfVectorSpace);
// special convenience macros
#define IS_BASED_EXTERIOR_POWER_OF_VECTOR_SPACE_UNIQUELY(Concept) HasUniqueBasedExteriorPowerOfVectorSpaceStructure_f<Concept>::V
#define AS_BASED_EXTERIOR_POWER_OF_VECTOR_SPACE(Concept) UniqueBasedExteriorPowerOfVectorSpaceStructureOf_f<Concept>::T

// TODO: thinka bout this
// template <typename ExteriorPowerOfVectorSpace, typename Basis>
// struct DualOf_f<BasedExteriorPowerOfVectorSpace_c<ExteriorPowerOfVectorSpace,Basis> >
// {
//     typedef BasedExteriorPowerOfVectorSpace_c<typename DualOf_f<ExteriorPowerOfVectorSpace>::T,typename DualOf_f<Basis>::T> T;
// };


// Factor_ must be a BasedVectorSpace_c type
template <Uint32 ORDER_, typename Factor_>
struct ExteriorPowerOfBasedVectorSpace_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_BASED_VECTOR_SPACE), };
    typedef typename TypeListWithMultiplicity_t<Factor_,ORDER_>::T FactorTypeList;

    typedef BasedExteriorPowerOfVectorSpace_c<ExteriorPowerOfVectorSpace_c<ORDER_,Factor_>,ExteriorPowerOfBasis_c<ORDER_,typename AS_BASED_VECTOR_SPACE(Factor_)::Basis> > As_BasedExteriorPowerOfVectorSpace;
    typedef EmbeddableInTensorProductOfBasedVectorSpaces_c<typename TensorPowerOfBasedVectorSpace_f<ORDER_,Factor_>::T,
                                                           typename TensorPowerOfVectorSpace_f<ORDER_,Factor_>::T> As_EmbeddableInTensorProductOfBasedVectorSpaces;
public:
    typedef TypeList_t<As_BasedExteriorPowerOfVectorSpace,
            TypeList_t<As_EmbeddableInTensorProductOfBasedVectorSpaces> > ParentTypeList;

    typedef typename As_BasedExteriorPowerOfVectorSpace::Id Id;
    typedef Factor_ Factor;

    static std::string type_as_string ()
    {
        return "ExteriorPowerOfBasedVectorSpace_c<" + AS_STRING(ORDER_) + ',' + TypeStringOf_t<Factor_>::eval() + '>';
    }
};

template <Uint32 ORDER_, typename Factor_>
struct IsConcept_f<ExteriorPowerOfBasedVectorSpace_c<ORDER_,Factor_> >
{ static bool const V = true; };

template <typename T> struct IsExteriorPowerOfBasedVectorSpace_f { static bool const V = false; };
template <Uint32 ORDER, typename Factor> struct IsExteriorPowerOfBasedVectorSpace_f<ExteriorPowerOfBasedVectorSpace_c<ORDER,Factor> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(ExteriorPowerOfBasedVectorSpace);
// special convenience macros
#define IS_EXTERIOR_POWER_OF_BASED_VECTOR_SPACE_UNIQUELY(Concept) HasUniqueExteriorPowerOfBasedVectorSpaceStructure_f<Concept>::V
#define AS_EXTERIOR_POWER_OF_BASED_VECTOR_SPACE(Concept) UniqueExteriorPowerOfBasedVectorSpaceStructureOf_f<Concept>::T

template <Uint32 ORDER_, typename Factor_>
struct DualOf_f<ExteriorPowerOfBasedVectorSpace_c<ORDER_,Factor_> >
{
    typedef BasedExteriorPowerOfVectorSpace_c<typename DualOf_f<ExteriorPowerOfVectorSpace_c<ORDER_,Factor_> >::T,
                                              typename DualOf_f<typename UniqueBasedVectorSpaceStructureOf_f<ExteriorPowerOfBasedVectorSpace_c<ORDER_,Factor_> >::T::Basis>::T> T;
};

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_EXTERIORPOWER_HPP_
