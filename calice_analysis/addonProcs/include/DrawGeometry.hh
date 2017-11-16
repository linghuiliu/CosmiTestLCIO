void DrawGeometry(){
   const gear::TPCParameters&  pTPC      = Global::GEAR->getTPCParameters();
   const gear::PadRowLayout2D& padLayout = pTPC.getPadLayout();
   const gear::DoubleVec&      planeExt  = padLayout.getPlaneExtent();
   float r_min_tpc = planeExt[0];
   float r_max_tpc = planeExt[1];
   float z_max_tpc = pTPC.getMaxDriftLength();
   const gear::CalorimeterParameters& pECAL_B = 
            Global::GEAR->getEcalBarrelParameters();
   float r_min_ecal_bar = pECAL_B.getExtent()[0];
   float r_max_ecal_bar = pECAL_B.getExtent()[1];
   //   float z_min_ecal_bar = pECAL_B.getExtent()[2];
   float z_max_ecal_bar = pECAL_B.getExtent()[3];
   const gear::CalorimeterParameters& pECAL_E = 
            Global::GEAR->getEcalEndcapParameters();
   //   float r_min_ecal_ecap = pECAL_E.getExtent()[0];
   float r_max_ecal_ecap = pECAL_E.getExtent()[1];
   float z_min_ecal_ecap = pECAL_E.getExtent()[2];
   float z_max_ecal_ecap = pECAL_E.getExtent()[3];
   const gear::CalorimeterParameters& pHCAL_B = 
            Global::GEAR->getHcalBarrelParameters();
   //  _innerHcalRadius = float(pHcalBarrel.getExtent()[0]);
   float r_min_hcal_bar = pHCAL_B.getExtent()[0];
   float r_max_hcal_bar = pHCAL_B.getExtent()[1];
   //   float z_min_hcal_bar = pHCAL_B.getExtent()[2];
   float z_max_hcal_bar = pHCAL_B.getExtent()[3];
   const gear::CalorimeterParameters& pHCAL_R = 
             Global::GEAR->getHcalRingParameters();
   //   float r_min_hcal_ring = pHCAL_R.getExtent()[0];
   float r_max_hcal_ring = pHCAL_R.getExtent()[1];
   float z_min_hcal_ring = pHCAL_R.getExtent()[2];
   float z_max_hcal_ring = pHCAL_R.getExtent()[3];
   const gear::CalorimeterParameters& pHCAL_E = 
            Global::GEAR->getHcalEndcapParameters();
   //   float r_min_hcal_ecap = pHCAL_E.getExtent()[0];
   float r_max_hcal_ecap = pHCAL_E.getExtent()[1];
   float z_min_hcal_ecap = pHCAL_E.getExtent()[2];
   float z_max_hcal_ecap = pHCAL_E.getExtent()[3];
// =======================================================================
//To convert inner radius of polygone to its outer radius
//   float Cos4  = cos(M_PI/4.0);
   float Cos8  = cos(M_PI/8.0);
   float Cos16 = cos(M_PI/16.);
// convertion of  inner radius of polygone to its outer radius
   float r_inn_ecal_bar     = r_min_ecal_bar/Cos8;  
   float r_out_ecal_bar     = (r_max_ecal_bar)/Cos8 ;
   //   float r_inn_ecal_ecap    = r_min_ecal_ecap/Cos4;
   float r_out_ecal_ecap    = r_max_ecal_ecap/Cos8;
   float thick_ecal_ecap    = 0.5*(z_max_ecal_ecap - z_min_ecal_ecap);
   float shift_ecal_z_plus  = z_min_ecal_ecap;
   float shift_ecal_z_minus = z_min_ecal_ecap + 2.0*thick_ecal_ecap;
   float r_inn_hcal_bar     = r_min_hcal_bar/Cos8;
   float r_out_hcal_bar     = r_max_hcal_bar/Cos16;
   //   float r_inn_hcal_ring    = r_min_hcal_ring/Cos4;
   float r_out_hcal_ring    = r_max_hcal_ring/Cos8;
   float thick_hcal_ring    = 0.5*(z_max_hcal_ring - 
		 z_min_hcal_ring + 20.0); // +20 by hand to see hits inside
   float shift_hcalr_z_plus  = z_min_hcal_ring;
   float shift_hcalr_z_minus = z_min_hcal_ring + 2.0*thick_hcal_ring;
   //   float r_inn_hcal_ecap    = r_min_hcal_ecap/Cos4;
   float r_out_hcal_ecap    = r_max_hcal_ecap/Cos8;
   float thick_hcal_ecap    = 0.5*(z_max_hcal_ecap - 
		 z_min_hcal_ecap + 20.0); // +20 by hand to see hits inside
   float shift_hcal_z_plus  = z_min_hcal_ecap;
   float shift_hcal_z_minus = z_min_hcal_ecap + 2.0*thick_hcal_ecap;
// ========================================================================
    static CED_GeoCylinder geoCylindersANY[] = {
      { r_min_tpc,        40, 0.0, z_max_tpc,       -z_max_tpc,          0xff      },
      { r_max_tpc,        40, 0.0, z_max_tpc,       -z_max_tpc,          0xff      },
      { r_inn_ecal_bar ,  8, 22.5, z_max_ecal_bar,  -z_max_ecal_bar,     0x7f7f1f  },
      { r_out_ecal_bar ,  8, 22.5, z_max_ecal_bar,  -z_max_ecal_bar,     0x7f7f1f  },
      { r_out_ecal_ecap,  8, 22.5, thick_ecal_ecap,  shift_ecal_z_plus,  0x7f7f1f  },
      { r_out_ecal_ecap,  8, 22.5, thick_ecal_ecap, -shift_ecal_z_minus, 0x7f7f1f  },
      { r_inn_hcal_bar ,  8, 22.5, z_max_hcal_bar,  -z_max_hcal_bar,     0x00cf00  },
      { r_out_hcal_bar , 16,  0.0, z_max_hcal_bar,  -z_max_hcal_bar,     0x00cf00  },
      { r_out_hcal_ring,  16,  0.0, thick_hcal_ring,  shift_hcalr_z_plus, 0x00cf00 },
      { r_out_hcal_ring,  16,  0.0, thick_hcal_ring, -shift_hcalr_z_minus,0x00cf00 },
      { r_out_hcal_ecap,  16,  0.0, thick_hcal_ecap,  shift_hcal_z_plus,  0x00cf00 },
      { r_out_hcal_ecap,  16,  0.0, thick_hcal_ecap, -shift_hcal_z_minus, 0x00cf00 } 
    };
// ========================================================================
      ced_geocylinders(sizeof(geoCylindersANY)/sizeof(CED_GeoCylinder),
		       geoCylindersANY);
} // End of DrawGeometry
