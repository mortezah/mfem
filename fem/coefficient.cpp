// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.

// Implementation of Coefficient class

#include "fem.hpp"

#include <cmath>
#include <limits>

namespace mfem
{

using namespace std;

double PWConstCoefficient::Eval(const ElementTransformation & T) const
{
   int att = T.Attribute;
   return (constants(att-1));
}

double FunctionCoefficient::Eval(const ElementTransformation & T) const
{
   MFEM_ASSERT(T.IntPointSet(), "Integration point not set.");

   double x[3];
   Vector transip(x, 3);

   T.Transform(T.GetIntPoint(), transip);

   if (Function)
   {
      return ((*Function)(transip));
   }
   else
   {
      return (*TDFunction)(transip, GetTime());
   }
}

double GridFunctionCoefficient::Eval (const ElementTransformation &T) const
{
   MFEM_ASSERT(T.IntPointSet(), "Integration point not set.");
   return GridF -> GetValue (T.ElementNo, T.GetIntPoint(), Component);
}

double TransformedCoefficient::Eval(const ElementTransformation &T) const
{
   if (Q2)
   {
      return (*Transform2)(Q1->Eval(T, GetTime()),
                           Q2->Eval(T, GetTime()));
   }
   else
   {
      return (*Transform1)(Q1->Eval(T, GetTime()));
   }
}

void DeltaCoefficient::SetDeltaCenter(const Vector& vcenter)
{
   MFEM_VERIFY(vcenter.Size() <= 3,
               "SetDeltaCenter::Maximum number of dim supported is 3")
   for (int i = 0; i < vcenter.Size(); i++) { center[i] = vcenter[i]; }
   sdim = vcenter.Size();
}

void DeltaCoefficient::GetDeltaCenter(Vector& vcenter) const
{
   vcenter.SetSize(sdim);
   vcenter = center;
}

double DeltaCoefficient::EvalDelta(const ElementTransformation &T) const
{
   double w = Scale();
   return weight ? weight->Eval(T, GetTime())*w : w;
}


void VectorCoefficient::Eval(DenseMatrix &M, ElementTransformation &T,
                             const IntegrationRule &ir) const
{
   Vector Mi;
   M.SetSize(vdim, ir.GetNPoints());
   for (int i = 0; i < ir.GetNPoints(); i++)
   {
      M.GetColumnReference(i, Mi);
      const IntegrationPoint &ip = ir.IntPoint(i);
      T.SetIntPoint(&ip);
      Eval(Mi, T);
   }
}

void VectorFunctionCoefficient::Eval(Vector &V,
                                     const ElementTransformation &T) const
{
   MFEM_ASSERT(T.IntPointSet(), "Integration point not set.");

   double x[3];
   Vector transip(x, 3);

   T.Transform(T.GetIntPoint(), transip);

   V.SetSize(vdim);
   if (Function)
   {
      (*Function)(transip, V);
   }
   else
   {
      (*TDFunction)(transip, GetTime(), V);
   }
   if (Q)
   {
      V *= Q->Eval(T, GetTime());
   }
}

VectorArrayCoefficient::VectorArrayCoefficient (int dim)
   : VectorCoefficient(dim), Coeff(dim)
{
   for (int i = 0; i < dim; i++)
   {
      Coeff[i] = NULL;
   }
}

VectorArrayCoefficient::~VectorArrayCoefficient()
{
   for (int i = 0; i < vdim; i++)
   {
      delete Coeff[i];
   }
}

void VectorArrayCoefficient::Eval(Vector &V,
                                  const ElementTransformation &T) const
{
   V.SetSize(vdim);
   for (int i = 0; i < vdim; i++)
   {
      V(i) = this->Eval(i, T);
   }
}

VectorGridFunctionCoefficient::VectorGridFunctionCoefficient (
   GridFunction *gf) : VectorCoefficient (gf -> VectorDim())
{
   GridFunc = gf;
}

void VectorGridFunctionCoefficient::Eval(Vector &V,
                                         const ElementTransformation &T) const
{
   MFEM_ASSERT(T.IntPointSet(), "Integration point not set.");
   GridFunc->GetVectorValue(T.ElementNo, T.GetIntPoint(), V);
}

void VectorGridFunctionCoefficient::Eval(
   DenseMatrix &M, ElementTransformation &T, const IntegrationRule &ir) const
{
   GridFunc->GetVectorValues(T, ir, M);
}

void VectorDeltaCoefficient::SetDirection(const Vector &_d)
{
   dir = _d;
   (*this).vdim = dir.Size();
}

void VectorDeltaCoefficient::EvalDelta(
   Vector &V, const ElementTransformation &T) const
{
   V = dir;
   V *= d.EvalDelta(T);
}

void VectorRestrictedCoefficient::Eval(Vector &V,
                                       const ElementTransformation &T) const
{
   V.SetSize(vdim);
   if (active_attr[T.Attribute-1])
   {
      c->SetTime(GetTime());
      c->Eval(V, T);
   }
   else
   {
      V = 0.0;
   }
}

void VectorRestrictedCoefficient::Eval(
   DenseMatrix &M, ElementTransformation &T, const IntegrationRule &ir) const
{
   if (active_attr[T.Attribute-1])
   {
      c->SetTime(GetTime());
      c->Eval(M, T, ir);
   }
   else
   {
      M.SetSize(vdim, ir.GetNPoints());
      M = 0.0;
   }
}

void MatrixFunctionCoefficient::Eval(DenseMatrix &K,
                                     const ElementTransformation &T) const
{
   MFEM_ASSERT(T.IntPointSet(), "Integration point not set.");

   double x[3];
   Vector transip(x, 3);

   T.Transform(T.GetIntPoint(), transip);

   K.SetSize(height, width);

   if (Function)
   {
      (*Function)(transip, K);
   }
   else if (TDFunction)
   {
      (*TDFunction)(transip, GetTime(), K);
   }
   else
   {
      K = mat;
   }
   if (Q)
   {
      K *= Q->Eval(T, GetTime());
   }
}

MatrixArrayCoefficient::MatrixArrayCoefficient (int dim)
   : MatrixCoefficient (dim)
{
   Coeff.SetSize(height*width);
   for (int i = 0; i < (height*width); i++)
   {
      Coeff[i] = NULL;
   }
}

MatrixArrayCoefficient::~MatrixArrayCoefficient ()
{
   for (int i=0; i < height*width; i++)
   {
      delete Coeff[i];
   }
}

void MatrixArrayCoefficient::Eval(DenseMatrix &K,
                                  const ElementTransformation &T) const
{
   for (int i = 0; i < height; i++)
   {
      for (int j = 0; j < width; j++)
      {
         K(i,j) = this->Eval(i, j, T);
      }
   }
}

void MatrixRestrictedCoefficient::Eval(DenseMatrix &K,
                                       const ElementTransformation &T) const
{
   if (active_attr[T.Attribute-1])
   {
      c->SetTime(GetTime());
      c->Eval(K, T);
   }
   else
   {
      K.SetSize(height, width);
      K = 0.0;
   }
}

double LpNormLoop(double p, Coefficient &coeff, Mesh &mesh,
                  const IntegrationRule *irs[])
{
   double norm = 0.0;
   ElementTransformation *tr;

   for (int i = 0; i < mesh.GetNE(); i++)
   {
      tr = mesh.GetElementTransformation(i);
      const IntegrationRule &ir = *irs[mesh.GetElementType(i)];
      for (int j = 0; j < ir.GetNPoints(); j++)
      {
         const IntegrationPoint &ip = ir.IntPoint(j);
         tr->SetIntPoint(&ip);
         double val = fabs(coeff.Eval(*tr));
         if (p < infinity())
         {
            norm += ip.weight * tr->Weight() * pow(val, p);
         }
         else
         {
            if (norm < val)
            {
               norm = val;
            }
         }
      }
   }
   return norm;
}

double LpNormLoop(double p, VectorCoefficient &coeff, Mesh &mesh,
                  const IntegrationRule *irs[])
{
   double norm = 0.0;
   ElementTransformation *tr;
   int vdim = coeff.GetVDim();
   Vector vval(vdim);
   double val;

   for (int i = 0; i < mesh.GetNE(); i++)
   {
      tr = mesh.GetElementTransformation(i);
      const IntegrationRule &ir = *irs[mesh.GetElementType(i)];
      for (int j = 0; j < ir.GetNPoints(); j++)
      {
         const IntegrationPoint &ip = ir.IntPoint(j);
         tr->SetIntPoint(&ip);
         coeff.Eval(vval, *tr);
         if (p < infinity())
         {
            for (int idim(0); idim < vdim; ++idim)
            {
               norm += ip.weight * tr->Weight() * pow(fabs( vval(idim) ), p);
            }
         }
         else
         {
            for (int idim(0); idim < vdim; ++idim)
            {
               val = fabs(vval(idim));
               if (norm < val)
               {
                  norm = val;
               }
            }
         }
      }
   }

   return norm;
}

double ComputeLpNorm(double p, Coefficient &coeff, Mesh &mesh,
                     const IntegrationRule *irs[])
{
   double norm = LpNormLoop(p, coeff, mesh, irs);

   if (p < infinity())
   {
      // negative quadrature weights may cause norm to be negative
      if (norm < 0.0)
      {
         norm = -pow(-norm, 1.0/p);
      }
      else
      {
         norm = pow(norm, 1.0/p);
      }
   }

   return norm;
}

double ComputeLpNorm(double p, VectorCoefficient &coeff, Mesh &mesh,
                     const IntegrationRule *irs[])
{
   double norm = LpNormLoop(p, coeff, mesh, irs);

   if (p < infinity())
   {
      // negative quadrature weights may cause norm to be negative
      if (norm < 0.0)
      {
         norm = -pow(-norm, 1.0/p);
      }
      else
      {
         norm = pow(norm, 1.0/p);
      }
   }

   return norm;
}

#ifdef MFEM_USE_MPI
double ComputeGlobalLpNorm(double p, Coefficient &coeff, ParMesh &pmesh,
                           const IntegrationRule *irs[])
{
   double loc_norm = LpNormLoop(p, coeff, pmesh, irs);
   double glob_norm = 0;

   MPI_Comm comm = pmesh.GetComm();

   if (p < infinity())
   {
      MPI_Allreduce(&loc_norm, &glob_norm, 1, MPI_DOUBLE, MPI_SUM, comm);

      // negative quadrature weights may cause norm to be negative
      if (glob_norm < 0.0)
      {
         glob_norm = -pow(-glob_norm, 1.0/p);
      }
      else
      {
         glob_norm = pow(glob_norm, 1.0/p);
      }
   }
   else
   {
      MPI_Allreduce(&loc_norm, &glob_norm, 1, MPI_DOUBLE, MPI_MAX, comm);
   }

   return glob_norm;
}

double ComputeGlobalLpNorm(double p, VectorCoefficient &coeff, ParMesh &pmesh,
                           const IntegrationRule *irs[])
{
   double loc_norm = LpNormLoop(p, coeff, pmesh, irs);
   double glob_norm = 0;

   MPI_Comm comm = pmesh.GetComm();

   if (p < infinity())
   {
      MPI_Allreduce(&loc_norm, &glob_norm, 1, MPI_DOUBLE, MPI_SUM, comm);

      // negative quadrature weights may cause norm to be negative
      if (glob_norm < 0.0)
      {
         glob_norm = -pow(-glob_norm, 1.0/p);
      }
      else
      {
         glob_norm = pow(glob_norm, 1.0/p);
      }
   }
   else
   {
      MPI_Allreduce(&loc_norm, &glob_norm, 1, MPI_DOUBLE, MPI_MAX, comm);
   }

   return glob_norm;
}
#endif

}
