// -*- C++ -*-

#ifndef DC_HIT_HH
#define DC_HIT_HH

#include <vector>
#include <deque>
#include <string>

#include <Rtypes.h>
#include <TObject.h>

#include <std_ostream.hh>

class DCLTrackHit;

//______________________________________________________________________________
class DCHit : public TObject
{
public:
  DCHit( void );
  DCHit( Int_t layer );
  DCHit( Int_t layer, Double_t wire );
  ~DCHit( void );

private:
  DCHit( const DCHit& );
  DCHit& operator =( const DCHit& );

private:
  Int_t                 m_layer;
  Double_t              m_wire;
  std::vector<Int_t>    m_tdc;
  std::vector<Int_t>    m_adc;
  std::vector<Int_t>    m_trailing;
  std::vector<Double_t> m_dt;
  std::vector<Double_t> m_dl;
  std::vector<Double_t> m_trailing_time;
  Double_t              m_wpos;
  Double_t              m_angle;
  std::deque<Bool_t>    m_belong_track;
  std::deque<Bool_t>    m_dl_range;
  ///// for MWPC
  Int_t                 m_cluster_size;
  Bool_t                m_mwpc_flag;
  Double_t              m_mwpc_wire;
  Double_t              m_mwpc_wpos;
  ///// for SSD
  Bool_t                m_is_ssd;
  Bool_t                m_zero_suppressed;
  Bool_t                m_time_corrected;
  Bool_t                m_good_waveform;
  Int_t                 m_pedestal;
  Int_t                 m_peak_height;
  Int_t                 m_peak_position;
  Double_t              m_deviation;
  Double_t              m_amplitude;
  Double_t              m_peak_time;
  Double_t              m_adc_sum;
  Double_t              m_de;
  Double_t              m_rms;
  Double_t              m_chisqr;
  std::vector<Double_t> m_time;
  std::vector<Double_t> m_waveform;
  Bool_t                m_belong_kaon;
  ///// for TOF
  Double_t              m_z;

  mutable std::vector <DCLTrackHit*> m_register_container;

public:
  Bool_t CalcDCObservables( void );
  Bool_t CalcMWPCObservables( void );
  Bool_t CalcFiberObservables( void );
  Bool_t CalcSsdObservables( void );
  //  Bool_t CalcObservablesSimulation( Double_t dlength);

  void SetLayer( Int_t layer )              { m_layer = layer; }
  void SetWire( Double_t wire )             { m_wire  = wire; }
  void SetTdcVal( Int_t tdc );
  void SetAdcVal( Int_t adc );
  void SetTdcTrailing( Int_t tdc )          { m_trailing.push_back(tdc); }
  void SetDriftTime( Double_t dt )          { m_dt.push_back(dt); }
  void ClearDriftTime( void )             { m_dt.clear(); }
  void SetDriftLength( Double_t dl )        { m_dl.push_back(dl); }
  void SetDriftLength( Int_t ith, Double_t dl ) { m_dl[ith] = dl; }
  void ClearDriftLength( void )           { m_dl.clear();}
  void SetTiltAngle( Double_t angleDegree ) { m_angle = angleDegree; }
  void SetTrailingTime( Double_t t )        { m_trailing_time.push_back(t); }

  void SetClusterSize( Int_t size )          { m_cluster_size = size;  }
  void SetMWPCFlag( Bool_t flag )            { m_mwpc_flag    = flag;  }
  void SetMeanWire( Double_t mwire )         { m_mwpc_wire    = mwire; }
  void SetMeanWirePosition( Double_t mwpos ) { m_mwpc_wpos    = mwpos; }
  void SetWirePosition( Double_t wpos )      { m_wpos         = wpos;  }

  ///// for SSD
  void SetSsdFlag( Bool_t flag=true )          { m_is_ssd        = flag;          }
  void SetGoodWaveForm( Bool_t good=true )     { m_good_waveform = good;          }
  void SetPedestal( Int_t pedestal )           { m_pedestal      = pedestal;      }
  void SetRms( Double_t rms )                  { m_rms           = rms;           }
  void SetAdcSum( Double_t sum )               { m_adc_sum       = sum;           }
  void SetDe( Double_t de )                    { m_de            = de;            }
  void SetDeviation( Double_t deviation )      { m_deviation     = deviation;     }
  void SetTime( std::vector<Double_t> time )             { m_time          = time;          }
  void SetWaveform( std::vector<Double_t> waveform )     { m_waveform      = waveform;      }
  void SetAmplitude( Double_t amplitude )      { m_amplitude     = amplitude;     }
  void SetPeakTime( Double_t peaktime )        { m_peak_time     = peaktime;      }
  void SetPeakHeight( Int_t height )           { m_peak_height   = height;        }
  void SetPeakPosition( Int_t position )       { m_peak_position = position;      }
  void SetChisquare( Double_t chisqr )         { m_chisqr        = chisqr;        }

  ///// for TOF
  void SetZ( Double_t z ) { m_z = z; }

  Int_t GetLayer( void ) const { return m_layer; }
  Double_t GetWire( void )  const {
    if( m_mwpc_flag ) return m_mwpc_wire;
    else return m_wire;
  }

  Int_t GetTdcSize( void ) const { return m_tdc.size(); }
  Int_t GetAdcSize( void ) const { return m_adc.size(); }
  Int_t GetDriftTimeSize( void ) const { return m_dt.size(); }
  Int_t GetDriftLengthSize( void ) const { return m_dl.size(); }
  Int_t GetTdcVal( Int_t nh=0 ) const { return m_tdc[nh]; }
  Int_t GetAdcVal( Int_t nh=0 ) const { return m_adc[nh]; }
  Int_t GetTdcTrailing( Int_t nh=0 ) const { return m_trailing[nh]; }
  Int_t GetTdcTrailingSize( void ) const { return m_trailing.size(); }

  Double_t GetResolution( void ) const;

  Double_t GetDriftTime( Int_t nh=0 ) const { return m_dt[nh]; }
  Double_t GetDriftLength( Int_t nh=0 ) const { return m_dl[nh]; }
  Double_t GetTrailingTime( Int_t nh=0 ) const { return m_trailing_time[nh]; }

  Double_t GetTiltAngle( void ) const { return m_angle; }
  Double_t GetWirePosition( void ) const {
    if( m_mwpc_flag ) return m_mwpc_wpos;
    else return m_wpos;
  }

  Int_t GetClusterSize( void ) const { return m_cluster_size; }
  Double_t GetMeamWire( void ) const { return m_mwpc_wire; }
  Double_t GetMeamWirePosition( void ) const { return m_mwpc_wpos; }

  ///// for SSD
  Bool_t    IsSsd( void )              const { return m_is_ssd;          }
  Bool_t    IsTimeCorrected( void )    const { return m_time_corrected;  }
  Bool_t    IsGoodWaveForm( void )     const { return m_good_waveform;   }
  Bool_t    IsZeroSuppressed( void )   const { return m_zero_suppressed; }
  Int_t     GetPedestal( void )        const { return m_pedestal;        }
  std::vector<Double_t> GetTime( void )            const { return m_time;            }
  std::vector<Double_t> GetWaveform( void )        const { return m_waveform;        }
  Double_t  GetAmplitude( void )       const { return m_amplitude;       }
  Double_t  GetDeviation( void )       const { return m_deviation;       }
  Double_t  GetAdcSum( void )          const { return m_adc_sum;         }
  Double_t  GetDe( void )              const { return m_de;              }
  Double_t  GetPeakTime( void )        const { return m_peak_time;       }
  Double_t  GetRms( void )             const { return m_rms;             }
  Double_t  GetAdcPeakHeight( void )   const { return m_peak_height;     }
  Double_t  GetAdcPeakPosition( void ) const { return m_peak_position;   }
  Double_t  GetChisquare( void )       const { return m_chisqr;          }
  Bool_t    DoTimeCorrection( Double_t offset );
  void      JoinKaonTrack( void ) { m_belong_kaon = true; }
  void      QuitKaonTrack( void ) { m_belong_kaon = false; }
  Bool_t    BelongToKaonTrack( void ) const { return m_belong_kaon; }

  ///// for TOF
  Double_t GetZ( void ) const { return m_z; }

  void JoinTrack( Int_t nh=0 ) { m_belong_track[nh] = true; }
  void QuitTrack( Int_t nh=0 ) { m_belong_track[nh] = false; }
  Bool_t BelongToTrack( Int_t nh=0 ) const { return m_belong_track[nh]; }
  Bool_t IsWithinRange( Int_t nh=0 ) const { return m_dl_range[nh]; }

  void RegisterHits( DCLTrackHit *hit ) const
  { m_register_container.push_back(hit); }

  Bool_t ReCalcDC( Bool_t applyRecursively=false ) { return CalcDCObservables(); }
  Bool_t ReCalcMWPC( Bool_t applyRecursively=false ) { return CalcMWPCObservables(); }
  void Print( Option_t* option="" ) const;

protected:
  void ClearRegisteredHits( void );

  ClassDef(DCHit,0);
};

//_____________________________________________________________________
inline std::ostream&
operator <<( std::ostream& ost, const DCHit& hit )
{
  hit.Print();
  return ost;
}

#endif
