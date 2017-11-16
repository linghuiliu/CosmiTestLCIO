#ifndef __HcalSetup_hh__
#define __HcalSetup_hh__

class AHCReader;
namespace EVENT {
  class LCCollection;
}
namespace CALICE{
  class DetectorTransformation;
}

class HcalSetup {
public:
  HcalSetup();
  HcalSetup( const char* );
  HcalSetup( EVENT::LCCollection* mPos, 
	     EVENT::LCCollection* dPos = 0, 
	     EVENT::LCCollection* mCon = 0 );

  ~HcalSetup();

  void read( const char* );

  void setPositions( EVENT::LCCollection* mPos, 
		     EVENT::LCCollection* dPos = 0 );
  void setConnections( EVENT::LCCollection* );

  void setAngle( const float );
  const float getAngle() ; //const;

  EVENT::LCCollection* getModulePositions() const;
  EVENT::LCCollection* getModuleConnections() const;
  EVENT::LCCollection* getDetectorPosition() const;

protected:
  void reset();
  //const CALICE::DetectorTransformation* detPos() const;

private:
  EVENT::LCCollection* _modPosCol;
  EVENT::LCCollection* _detPosCol;
  EVENT::LCCollection* _modConCol;

  CALICE::DetectorTransformation* detPos();

  template< class T >
  EVENT::LCCollection* cloneCollection( EVENT::LCCollection* ) const;

  const float deg2rad( const float a ) const;
  const float rad2deg( const float a ) const;
};  // class HcalSetup

#endif
