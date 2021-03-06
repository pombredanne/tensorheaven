// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/diagonalbased2tensorproduct.hpp by Victor Dods, created 2013/07/24
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_DIAGONALBASED2TENSORPRODUCT_HPP_
#define TENH_CONCEPTUAL_DIAGONALBASED2TENSORPRODUCT_HPP_

#include "tenh/core.hpp"

#include <type_traits>

#include "tenh/componentindex.hpp" // technically not conceptual code, but close enough.
#include "tenh/conceptual/concept.hpp"
#include "tenh/conceptual/embeddableintensorproduct.hpp"
#include "tenh/conceptual/linearembedding.hpp"
#include "tenh/conceptual/symmetricpower.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/multiindex.hpp" // technically not conceptual code, but close enough.

namespace Tenh {

// ///////////////////////////////////////////////////////////////////////////
// Diagonal2TensorProduct_c
// ///////////////////////////////////////////////////////////////////////////

// formal "diagonal 2-tensor product" of symbols
template <typename Factor0_, typename Factor1_>
struct Diagonal2TensorProduct_c
{
    typedef Typle_t<> ParentTyple;

    typedef Factor0_ Factor0;
    typedef Factor1_ Factor1;

    static std::string type_as_string (bool verbose)
    {
        if (verbose)
            return "Diagonal2TensorProduct_c<" + type_string_of<Factor0_>() + ',' + type_string_of<Factor1_>() + '>';
        else
            return "diag2(" + terse_string_of<Factor0_>() + ',' + terse_string_of<Factor1_>() + ')';
    }
};

template <typename Factor0_, typename Factor1_>
struct IsConcept_f<Diagonal2TensorProduct_c<Factor0_,Factor1_>>
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDiagonal2TensorProduct_f
{
    static bool const V = false;
private:
    IsDiagonal2TensorProduct_f();
};

template <typename Factor0_, typename Factor1_>
struct IsDiagonal2TensorProduct_f<Diagonal2TensorProduct_c<Factor0_,Factor1_>>
{
    static bool const V = true;
private:
    IsDiagonal2TensorProduct_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(Diagonal2TensorProduct);
// special convenience macros
#define IS_DIAGONAL_2_TENSOR_PRODUCT_UNIQUELY(Concept) HasUniqueDiagonal2TensorProductStructure_f<Concept>::V
#define AS_DIAGONAL_2_TENSOR_PRODUCT(Concept) UniqueDiagonal2TensorProductStructureOf_f<Concept>::T

// ///////////////////////////////////////////////////////////////////////////
// Diagonal2TensorProductOfBases_c
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor0_, typename Factor1_>
struct Diagonal2TensorProductOfBases_c
{
    static_assert(IS_BASIS_UNIQUELY(Factor0_), "Factor0_ must have unique basis structure");
    static_assert(IS_BASIS_UNIQUELY(Factor1_), "Factor1_ must have unique basis structure");
private:
    typedef Basis_c<Diagonal2TensorProduct_c<Factor0_,Factor1_>> As_Basis;
    typedef Diagonal2TensorProduct_c<Factor0_,Factor1_> As_Diagonal2TensorProduct;
public:
    typedef Typle_t<As_Basis,As_Diagonal2TensorProduct> ParentTyple;

    typedef typename As_Basis::Id Id;
    typedef Factor0_ Factor0;
    typedef Factor1_ Factor1;

    static std::string type_as_string (bool verbose)
    {
        if (verbose)
            return "Diagonal2TensorProductOfBases_c<" + type_string_of<Factor0_>() + ',' + type_string_of<Factor1_>() + '>';
        else
            return "diag2(" + terse_string_of<Factor0_>() + ',' + terse_string_of<Factor1_>() + ')';
    }
};

template <typename Factor0_, typename Factor1_>
struct IsConcept_f<Diagonal2TensorProductOfBases_c<Factor0_,Factor1_>>
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDiagonal2TensorProductOfBases_f
{
    static bool const V = false;
private:
    IsDiagonal2TensorProductOfBases_f();
};

template <typename Factor0_, typename Factor1_>
struct IsDiagonal2TensorProductOfBases_f<Diagonal2TensorProductOfBases_c<Factor0_,Factor1_>>
{
    static bool const V = true;
private:
    IsDiagonal2TensorProductOfBases_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(Diagonal2TensorProductOfBases);
// special convenience macros
#define IS_DIAGONAL_2_TENSOR_PRODUCT_OF_BASES_UNIQUELY(Concept) HasUniqueDiagonal2TensorProductOfBasesStructure_f<Concept>::V
#define AS_DIAGONAL_2_TENSOR_PRODUCT_OF_BASES(Concept) UniqueDiagonal2TensorProductOfBasesStructureOf_f<Concept>::T

// ///////////////////////////////////////////////////////////////////////////
// Diagonal2TensorProductOfBasedVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor0_, typename Factor1_>
struct Diagonal2TensorProductOfBasedVectorSpaces_c
{
    static_assert(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor0_), "Factor0_ must have unique based vector space structure");
    static_assert(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor1_), "Factor1_ must have unique based vector space structure");
    static_assert(TypesAreEqual_f<typename ScalarFieldOf_f<Factor0_>::T,typename ScalarFieldOf_f<Factor1_>::T>::V, "all factors must have the same scalar field");
private:
    typedef Typle_t<Factor0_,Factor1_> FactorTyple;

    static Uint32 const VECTOR_SPACE_DIMENSION = (DimensionOf_f<Factor0_>::V < DimensionOf_f<Factor1_>::V) ? DimensionOf_f<Factor0_>::V : DimensionOf_f<Factor1_>::V;
    typedef VectorSpace_c<typename ScalarFieldOf_f<Factor0_>::T,
                          VECTOR_SPACE_DIMENSION,
                          Diagonal2TensorProduct_c<Factor0_,Factor1_>> UnderlyingVectorSpace;

    typedef BasedVectorSpace_c<UnderlyingVectorSpace,
                               Diagonal2TensorProductOfBases_c<typename BasisOf_f<Factor0_>::T,
                                                               typename BasisOf_f<Factor1_>::T>> As_BasedVectorSpace;
    typedef EmbeddableInTensorProductOfBasedVectorSpaces_c<TensorProductOfBasedVectorSpaces_c<FactorTyple>,
                                                           TensorProductOfVectorSpaces_c<FactorTyple>> As_EmbeddableInTensorProductOfBasedVectorSpaces;
public:
    typedef Typle_t<As_BasedVectorSpace,As_EmbeddableInTensorProductOfBasedVectorSpaces> ParentTyple;

    typedef typename As_BasedVectorSpace::Id Id;
    typedef Factor0_ Factor0;
    typedef Factor1_ Factor1;

    static std::string type_as_string (bool verbose)
    {
        if (verbose)
            return "Diagonal2TensorProductOfBasedVectorSpaces_c<" + type_string_of<Factor0_>() + ',' + type_string_of<Factor1_>() + '>';
        else
            return "diag2(" + terse_string_of<Factor0_>() + ',' + terse_string_of<Factor1_>() + ')';
    }
};

template <typename Factor0_, typename Factor1_>
struct IsConcept_f<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>>
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDiagonal2TensorProductOfBasedVectorSpaces_f
{
    static bool const V = false;
private:
    IsDiagonal2TensorProductOfBasedVectorSpaces_f();
};

template <typename Factor0_, typename Factor1_>
struct IsDiagonal2TensorProductOfBasedVectorSpaces_f<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>>
{
    static bool const V = true;
private:
    IsDiagonal2TensorProductOfBasedVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(Diagonal2TensorProductOfBasedVectorSpaces);
// special convenience macros
#define IS_DIAGONAL_2_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueDiagonal2TensorProductOfBasedVectorSpacesStructure_f<Concept>::V
#define AS_DIAGONAL_2_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(Concept) UniqueDiagonal2TensorProductOfBasedVectorSpacesStructureOf_f<Concept>::T

// TODO: verify that this mathematical claim is true
template <typename Factor0, typename Factor1>
struct DualOf_f<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0,Factor1>>
{
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<typename DualOf_f<Factor0>::T,typename DualOf_f<Factor1>::T> T;
private:
    DualOf_f();
};

// convenience metafunction for using a FactorTyple_ instead of two separate factors
template <typename FactorTyple_>
struct Diagonal2TensorProductOfBasedVectorSpaces_f
{
    static_assert(Length_f<FactorTyple_>::V == 2, "there must be exactly 2 factors");
private:
    typedef typename Element_f<FactorTyple_,0>::T Factor0;
    typedef typename Element_f<FactorTyple_,1>::T Factor1;
    Diagonal2TensorProductOfBasedVectorSpaces_f();
public:
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0,Factor1> T;
};

// specialization for FactorTypleOf_f
template <typename Factor0_, typename Factor1_>
struct BaseProperty_f<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>,FactorTyple>
{
    typedef Typle_t<Factor0_,Factor1_> T;
private:
    BaseProperty_f();
};

// ///////////////////////////////////////////////////////////////////////////
// helper functions for easing use of the type system
// ///////////////////////////////////////////////////////////////////////////

// some helper macros for the following different versions of diag2
#define BOTH_FACTORS_ARE_BASES_UNIQUELY(Factor0, Factor1) (IS_BASIS_UNIQUELY(Factor0) && IS_BASIS_UNIQUELY(Factor1))
#define BOTH_FACTORS_ARE_BASED_VECTOR_SPACES_UNIQUELY(Factor0, Factor1) (IS_BASED_VECTOR_SPACE_UNIQUELY(Factor0) && IS_BASED_VECTOR_SPACE_UNIQUELY(Factor1))

// formal diagonal 2-tensor product
template <typename Factor0_, typename Factor1_>
typename std::enable_if<(!BOTH_FACTORS_ARE_BASES_UNIQUELY(Factor0_,Factor1_) &&
                         !BOTH_FACTORS_ARE_BASED_VECTOR_SPACES_UNIQUELY(Factor0_,Factor1_)),
                        Diagonal2TensorProduct_c<Factor0_,Factor1_>>::type
    diag2 (Factor0_ const &, Factor1_ const &)
{
    return Diagonal2TensorProduct_c<Factor0_,Factor1_>();
}

// diagonal 2-tensor product of bases
template <typename Factor0_, typename Factor1_>
typename std::enable_if<(BOTH_FACTORS_ARE_BASES_UNIQUELY(Factor0_,Factor1_) &&
                         !BOTH_FACTORS_ARE_BASED_VECTOR_SPACES_UNIQUELY(Factor0_,Factor1_)),
                        Diagonal2TensorProductOfBases_c<Factor0_,Factor1_>>::type
    diag2 (Factor0_ const &, Factor1_ const &)
{
    return Diagonal2TensorProductOfBases_c<Factor0_,Factor1_>();
}

// diagonal 2-tensor product of based vector spaces
template <typename Factor0_, typename Factor1_>
typename std::enable_if<(!BOTH_FACTORS_ARE_BASES_UNIQUELY(Factor0_,Factor1_) &&
                         BOTH_FACTORS_ARE_BASED_VECTOR_SPACES_UNIQUELY(Factor0_,Factor1_)),
                        Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>>::type
    diag2 (Factor0_ const &, Factor1_ const &)
{
    return Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>();
}

#undef BOTH_FACTORS_ARE_BASES_UNIQUELY
#undef BOTH_FACTORS_ARE_BASED_VECTOR_SPACES_UNIQUELY

// for now, just do diagonal 2-tensor product of based vector spaces
template <typename Factor0_, typename Factor1_>
Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>
    diag2 (TensorProductOfBasedVectorSpaces_c<Typle_t<Factor0_,Factor1_>> const &)
{
    return Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>();
}

// ///////////////////////////////////////////////////////////////////////////
// linear embedding of diagonal 2-tensor into corresponding tensor product
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor0_, typename Factor1_, typename Scalar_, WithExceptions WITH_EXCEPTIONS_>
struct LinearEmbedding_c<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>,
                         TensorProductOfBasedVectorSpaces_c<Typle_t<Factor0_,Factor1_>>,
                         Scalar_,
                         NaturalEmbedding,
                         WITH_EXCEPTIONS_>
{
private:
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_> Diag2;
    typedef TensorProductOfBasedVectorSpaces_c<Typle_t<Factor0_,Factor1_>> Tensor2;
    static bool const FACTOR0DIM_LEQ_FACTOR1DIM = DimensionOf_f<Factor0_>::V <= DimensionOf_f<Factor1_>::V;
public:
    typedef ComponentIndex_t<DimensionOf_f<Diag2>::V> Diag2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Tensor2>::V> Tensor2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor0_>::V> Factor0ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor1_>::V> Factor1ComponentIndex;

    struct CoembedIndexIterator
    {
        CoembedIndexIterator (Diag2ComponentIndex const &i) : m(i.value() * (DimensionOf_f<Factor1_>::V+1), CheckRange::FALSE) { }
        void operator ++ () { m.set_to_end(); } // there is only one Tensor2ComponentIndex per Diag2ComponentIndex
        bool is_not_at_end () const { return m.is_not_at_end(); }
        Scalar_ scale_factor () const { return Scalar_(1); }
        typedef Tensor2ComponentIndex ComponentIndexReturnType;
        ComponentIndexReturnType const &component_index () const { return m; }
    private:
        Tensor2ComponentIndex m;
    };

private:
    typedef MultiIndex_t<Typle_t<Factor0ComponentIndex,Factor1ComponentIndex>> Tensor2MultiIndex;
public:
    static bool embedded_component_is_procedural_zero (Tensor2ComponentIndex const &i)
    {
        Tensor2MultiIndex m(i); // does the row-major indexing conversion
        // it's a procedural zero if the component is off the diagonal.
        return m.template el<0>().value() != m.template el<1>().value();
    }
    static Scalar_ scalar_factor_for_embedded_component (Tensor2ComponentIndex const &i)
    {
        if (bool(WITH_EXCEPTIONS_) && embedded_component_is_procedural_zero(i))
            throw std::domain_error(FORMAT(i.value()) + " is not in the domain of scalar_factor_for_embedded_component");

        return Scalar_(1);
    }
    static Diag2ComponentIndex source_component_index_for_embedded_component (Tensor2ComponentIndex const &i)
    {
        if (bool(WITH_EXCEPTIONS_) && embedded_component_is_procedural_zero(i))
            throw std::domain_error(FORMAT(i.value()) + " is not in the domain of scalar_factor_for_embedded_component");

        Tensor2MultiIndex m(i); // does the row-major indexing conversion
        return m.template el<(FACTOR0DIM_LEQ_FACTOR1DIM ? 0 : 1)>();
    }
};

// ///////////////////////////////////////////////////////////////////////////
// linear embedding of diagonal 2-tensor of uniform factor type into
// corresponding 2nd symmetric power
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor_, typename Scalar_, WithExceptions WITH_EXCEPTIONS_>
struct LinearEmbedding_c<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor_,Factor_>,
                         SymmetricPowerOfBasedVectorSpace_c<2,Factor_>,
                         Scalar_,
                         NaturalEmbedding,
                         WITH_EXCEPTIONS_>
{
private:
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<Factor_,Factor_> Diag2;
    typedef SymmetricPowerOfBasedVectorSpace_c<2,Factor_> Sym2;
public:
    typedef ComponentIndex_t<DimensionOf_f<Diag2>::V> Diag2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Sym2>::V> Sym2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor_>::V> FactorComponentIndex;

    struct CoembedIndexIterator
    {
        CoembedIndexIterator (Diag2ComponentIndex const &i) : m(i.value() + (i.value()*(i.value()+1))/2, CheckRange::FALSE) { }
        void operator ++ () { m.set_to_end(); } // there is only one Sym2ComponentIndex per Diag2ComponentIndex
        bool is_not_at_end () const { return m.is_not_at_end(); }
        Scalar_ scale_factor () const { return Scalar_(1); }
        typedef Sym2ComponentIndex ComponentIndexReturnType;
        ComponentIndexReturnType const &component_index () const { return m; }
    private:
        Sym2ComponentIndex m;
    };

    static bool embedded_component_is_procedural_zero (Sym2ComponentIndex const &i)
    {
        // Sym2 uses the lower-triangular indexing (which doesn't depend on the dimension of Factor_)
        Uint32 t = index_of_greatest_simplicial_number_leq(i.value(), 2);
        // std::cout << FORMAT_VALUE(i.value()) << ", " << FORMAT_VALUE(t) << '\n';
        FactorComponentIndex row(t-1, CheckRange::FALSE);
        FactorComponentIndex col(i.value() - t*(t-1)/2, CheckRange::FALSE);
        // off-diagonals are procedural zeros
        return row.value() != col.value();
    }
    static Scalar_ scalar_factor_for_embedded_component (Sym2ComponentIndex const &i)
    {
        if (bool(WITH_EXCEPTIONS_) && embedded_component_is_procedural_zero(i))
            throw std::domain_error(FORMAT(i.value()) + " is not in the domain of scalar_factor_for_embedded_component");

        return Scalar_(1);
    }
    static Diag2ComponentIndex source_component_index_for_embedded_component (Sym2ComponentIndex const &i)
    {
        if (bool(WITH_EXCEPTIONS_) && embedded_component_is_procedural_zero(i))
            throw std::domain_error(FORMAT(i.value()) + " is not in the domain of scalar_factor_for_embedded_component");

        // Sym2 uses the lower-triangular indexing (which doesn't depend on the dimension of Factor_)
        Uint32 t = index_of_greatest_simplicial_number_leq(i.value(), 2);
        FactorComponentIndex row(t-1, CheckRange::FALSE);
        FactorComponentIndex col(i.value() - t*(t-1)/2, CheckRange::FALSE);
        // FactorComponentIndex and Diag2ComponentIndex are the same C++ type by dimensionality
        return row;
    }
};

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_DIAGONALBASED2TENSORPRODUCT_HPP_
