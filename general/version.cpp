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

#include "../config/config.hpp"
#include "version.hpp"

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

namespace mfem
{


int get_version()
{
   return MFEM_VERSION;
}


int get_version_major()
{
   return MFEM_VERSION_MAJOR;
}


int get_version_minor()
{
   return MFEM_VERSION_MINOR;
}


int get_version_patch()
{
   return MFEM_VERSION_PATCH;
}


const char *get_version_str()
{
   static const char *version_str = EXPAND_AND_QUOTE(MFEM_VERSION);
   return version_str;
}


const char *get_config_str()
{
   static const char *config_str =
      ""
#ifdef MFEM_USE_MPI
      "MFEM_USE_MPI\n"
#endif
#ifdef MFEM_USE_METIS
      "MFEM_USE_METIS\n"
#endif
#ifdef MFEM_USE_METIS_5
      "MFEM_USE_METIS_5\n"
#endif
#ifdef MFEM_DEBUG
      "MFEM_DEBUG\n"
#endif
#ifdef MFEM_USE_GZSTREAM
      "MFEM_USE_GZSTREAM\n"
#endif
#ifdef MFEM_USE_LIBUNWIND
      "MFEM_USE_LIBUNWIND\n"
#endif
#ifdef MFEM_USE_LAPACK
      "MFEM_USE_LAPACK\n"
#endif
#ifdef MFEM_THREAD_SAFE
      "MFEM_THREAD_SAFE\n"
#endif
#ifdef MFEM_USE_OPENMP
      "MFEM_USE_OPENMP\n"
#endif
#ifdef MFEM_USE_MEMALLOC
      "MFEM_USE_MEMALLOC\n"
#endif
#ifdef MFEM_USE_SUNDIALS
      "MFEM_USE_SUNDIALS\n"
#endif
#ifdef MFEM_USE_MESQUITE
      "MFEM_USE_MESQUITE\n"
#endif
#ifdef MFEM_USE_SUITESPARSE
      "MFEM_USE_SUITESPARSE\n"
#endif
#ifdef MFEM_USE_SUPERLU
      "MFEM_USE_SUPERLU\n"
#endif
#ifdef MFEM_USE_STRUMPACK
      "MFEM_USE_STRUMPACK\n"
#endif
#ifdef MFEM_USE_GECKO
      "MFEM_USE_GECKO\n"
#endif
#ifdef MFEM_USE_GNUTLS
      "MFEM_USE_GNUTLS\n"
#endif
#ifdef MFEM_USE_NETCDF
      "MFEM_USE_NETCDF\n"
#endif
#ifdef MFEM_USE_PETSC
      "MFEM_USE_PETSC\n"
#endif
#ifdef MFEM_USE_MPFR
      "MFEM_USE_MPFR\n"
#endif
#ifdef MFEM_USE_SIDRE
      "MFEM_USE_SIDRE\n"
#endif
      "MFEM_TIMER_TYPE = " EXPAND_AND_QUOTE(MFEM_TIMER_TYPE)
      ;

   return config_str;
}

}
