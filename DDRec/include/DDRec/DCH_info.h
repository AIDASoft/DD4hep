#ifndef DDREC_DCH_INFO_H
#define DDREC_DCH_INFO_H

#include "TMath.h"

#include "DDRec/DetectorData.h"
#include <map>

namespace dd4hep {  namespace rec {


/* Data structure to store DCH geometry parameters
 *
 * Global parameters are members of this class
 *
 * Parameters of each layer are stored in the helper
 * class DCH_info_layer.
 *
 * The member database is a container which stores one
 * DCH_info_layer object per layer.
 *
 * To use this class, instantiate an object and define the global parameters.
 * Then call the method BuildLayerDatabase, which calculates
 * the parameters of each layer based on the global parameters.
 *
 * @author A. Tolosa Delgado, CERN
 * @date April 2024
 * @version Drift chamber v2
 *
 */
struct DCH_info_struct
{
public:
    // use alias of types to show more clearly what the variable is
    // if everything is double, the code is not readable
    /// type for layer number
    using DCH_layer = int;
    /// tpye for lengths
    using DCH_length_t = double;
    /// tpye for angles
    using DCH_angle_t = double;
    /// Half length of the active volume
    DCH_length_t Lhalf = {0};
    /// Inner radius of the active volume
    DCH_length_t rin = {0};
    /// Outer radius of the active volume
    DCH_length_t rout = {0};

    /// Inner guard wires radius
    DCH_length_t guard_inner_r_at_z0 = {0};
    /// Outer guard wires radius
    DCH_length_t guard_outer_r_at_zL2 = {0};

    /// number of cells of first layer
    int ncell0 = {0};
    /// increment the number of cells for each superlayer as:
    ///   ncells(ilayer) = dch_ncell0 + increment*superlayer(ilayer)
    ///   See DCH_info::Get_nsuperlayer_minus_1(ilayer)
    int ncell_increment = {0};

    /// cells within the same layer may be grouped into sectors, not in use atm
    int ncell_per_sector = {0};

    /// input number of layers in each superlayer
    DCH_layer nlayersPerSuperlayer = {0};
    /// input number of superlayers
    /// superlayer is an abstract level of grouping layers used to
    /// parametrize the increment of cells in each layer
    DCH_layer nsuperlayers = {0};
    /// Calculated as dch_nlayersPerSuperlayer * dch_nsuperlayers
    DCH_layer nlayers = {0};

    /// global twist angle
    /// alternating layers will change its sign
    DCH_angle_t  twist_angle = {0};

    /// Cell width for the first layer
    double first_width = {0};
    /// Cell radius for the first layer
    DCH_length_t first_sense_r = {0};

    void Set_lhalf(DCH_length_t _dch_Lhalf){Lhalf=_dch_Lhalf;}
    void Set_rin  (DCH_length_t _dch_rin  ){rin  = _dch_rin; }
    void Set_rout (DCH_length_t _dch_rout ){rout = _dch_rout;}

    void Set_guard_rin_at_z0  (DCH_length_t _dch_rin_z0_guard  ){guard_inner_r_at_z0  = _dch_rin_z0_guard;   }
    void Set_guard_rout_at_zL2(DCH_length_t _dch_rout_zL2_guard){guard_outer_r_at_zL2 = _dch_rout_zL2_guard; }

    void Set_ncell0              (int _ncell0              ){ncell0               = _ncell0;              }
    void Set_ncell_increment     (int _ncell_increment     ){ncell_increment      = _ncell_increment;     }

    void Set_nlayersPerSuperlayer(int _nlayersPerSuperlayer){nlayersPerSuperlayer = _nlayersPerSuperlayer;}
    void Set_nsuperlayers        (int _nsuperlayers        ){nsuperlayers         = _nsuperlayers;        }

    void Set_ncell_per_sector(int _ncell_per_sector){ncell_per_sector = _ncell_per_sector;}

    void Set_twist_angle (DCH_length_t _dch_twist_angle ){twist_angle = _dch_twist_angle;}

    void Set_first_width  (double _first_width  ){first_width   = _first_width;   }
    void Set_first_sense_r(double _first_sense_r){first_sense_r = _first_sense_r; }


    /// Get number of cells in a given layer
    ///   ncells = number of wires/2
    int Get_ncells(int ilayer){return database.at(ilayer).nwires/2;}

    /// Get phi width for the twisted tube and the step (phi distance between cells)
    DCH_angle_t Get_phi_width(int ilayer){return (TMath::TwoPi()/Get_ncells(ilayer))*dd4hep::rad;}

    /// phi positioning, adding offset for odd ilayers
    /// there is a staggering in phi for alternating layers, 0.25*cell_phi_width*(ilayer%2);
    DCH_angle_t Get_cell_phi_angle(int ilayer, int nphi){ return (Get_phi_width(ilayer) * (nphi + 0.25*(ilayer%2)));}

    /// calculate superlayer for a given ilayer.
    /// WARNING: division of integers on purpose!
    int Get_nsuperlayer_minus_1(int ilayer){ return int((ilayer-1)/nlayersPerSuperlayer);}

    /// Calculate radius at z=L/2 given at z=0
    DCH_length_t Radius_zLhalf(DCH_length_t r_z0) const {
        return r_z0/cos(twist_angle/2/dd4hep::rad);
    }

    /// tan(stereoangle) = R(z=0)   / (L/2) * tan( twist_angle/2)
    DCH_angle_t stereoangle_z0(DCH_length_t r_z0) const {
        return atan( r_z0/Lhalf*tan(twist_angle/2/dd4hep::rad));
    }

    /// tan(stereoangle) = R(z=L/2) / (L/2) * sin( twist_angle/2)
    DCH_angle_t stereoangle_zLhalf(DCH_length_t r_zLhalf) const {
        return atan( r_zLhalf/Lhalf*sin(twist_angle/2/dd4hep::rad));
    }

    /// WireLength = 2*dch_Lhalf/cos(atan(Pitch_z0(r_z0)/(2*dch_Lhalf)))/cos(stereoangle_z0(r_z0))
    DCH_length_t WireLength(int nlayer, DCH_length_t r_z0) const {
        auto Pitch_z0 = database.at(nlayer).Pitch_z0(r_z0);
        return  2*Lhalf/cos(atan(Pitch_z0/(2*Lhalf)))/cos(stereoangle_z0(r_z0)/dd4hep::rad) ;
    };

    /// Internal helper struct for defining the layer layout
    struct DCH_info_layer
    {
        /// layer number
        DCH_layer layer = {0};
        /// 2x number of cells in that layer
        int nwires = {0};
        /// cell parameter
        double height_z0 = {0.};
        /// cell parameter
        double width_z0 = {0.};

        /// radius (cylindral coord) of sensitive wire
        DCH_length_t radius_sw_z0 = {0.};

        /// radius (cylindral coord) of 'down' field wires
        DCH_length_t radius_fdw_z0 = {0.};

        /// radius (cylindral coord) of 'up' field wires
        DCH_length_t radius_fuw_z0 = {0.};

        /// some quantities are derived from previous-layer ones
        ///  stereo angle is positive for odd layer number
        bool IsStereoPositive() const {
            return (1 == layer%2);
        }
        /// calculate sign based on IsStereoPositive
        int StereoSign() const {
            return (IsStereoPositive()*2 - 1);
        }

        /// separation between wires (along the circle)
        DCH_length_t Pitch_z0(DCH_length_t r_z0) const {
            return TMath::TwoPi()*r_z0/nwires;
        };

    };
    /// map to store parameters for each layer
    std::map<DCH_layer, DCH_info_layer> database;
    bool IsDatabaseEmpty() const { return database.empty(); }

    inline void BuildLayerDatabase();
    inline void Show_DCH_info_database(std::ostream& io) const;


};
typedef StructExtension<DCH_info_struct> DCH_info ;
inline std::ostream& operator<<( std::ostream& io , const DCH_info& d ){d.Show_DCH_info_database(io); return io;}

inline void DCH_info_struct::BuildLayerDatabase()
{
    // do not fill twice the database
    if( not this->IsDatabaseEmpty() ) return;

    auto ff_check_positive_parameter = [](double p, std::string pname) -> void {
        if(p<=0)throw std::runtime_error(Form("DCH: %s must be positive",pname.c_str()));
        return;
    };
    ff_check_positive_parameter(this->rin  ,"inner radius");
    ff_check_positive_parameter(this->rout ,"outer radius");
    ff_check_positive_parameter(this->Lhalf,"half length" );

    ff_check_positive_parameter(this->guard_inner_r_at_z0 ,"inner radius of guard wires" );
    ff_check_positive_parameter(this->guard_outer_r_at_zL2,"outer radius of guard wires" );


    ff_check_positive_parameter(this->ncell0,"ncells in the first layer" );
    ff_check_positive_parameter(this->ncell_increment,"ncells increment per superlayer" );
    ff_check_positive_parameter(this->ncell_per_sector,"ncells per sector" );

    // if dch_ncell_per_sector is not divisor of dch_ncell0 and dch_ncell_increment
    // throw an error
    if( 0 != (ncell0 % ncell_per_sector) || 0 != (ncell_increment % ncell_per_sector) )
        throw std::runtime_error("dch_ncell_per_sector is not divisor of dch_ncell0 or dch_ncell_increment");

    ff_check_positive_parameter(this->nsuperlayers,"number of superlayers" );
    ff_check_positive_parameter(this->nlayersPerSuperlayer,"number of layers per superlayer" );

    /// nlayers = nsuperlayers * nlayersPerSuperlayer
    /// default: 112 = 14 * 8
    this->nlayers = this->nsuperlayers * this->nlayersPerSuperlayer;

    ff_check_positive_parameter(this->first_width,"width of first layer cells" );
    ff_check_positive_parameter(this->first_sense_r,"radius of first layer cells" );


    // intialize layer 1 from input parameters
    {
        DCH_info_layer layer1_info;
        layer1_info.layer         = 1;
        layer1_info.nwires        = 2*this->ncell0;
        layer1_info.height_z0     = first_width;
        layer1_info.radius_sw_z0  = first_sense_r;
        layer1_info.radius_fdw_z0 = first_sense_r - 0.5*first_width;
        layer1_info.radius_fuw_z0 = first_sense_r + 0.5*first_width;
        layer1_info.width_z0      = TMath::TwoPi()*first_sense_r/this->ncell0;

        this->database.emplace(layer1_info.layer, layer1_info);
    }

    // some parameters of the following layer are calculated based on the previous ones
    // the rest are left as methods of DCH_info or DCH_info_layer class
    // loop over all layers, skipping the first one
    for(int ilayer = 2; ilayer<= this->nlayers; ++ilayer)
    {
        // initialize empty object, parameters are set later
        DCH_info_layer layer_info;

        // the loop counter actually corresponds to the layer number
        layer_info.layer = ilayer;
        // nwires is twice the number of cells in this particular layer (ilayer)
        layer_info.nwires = 2*(this->ncell0 + this->ncell_increment*Get_nsuperlayer_minus_1(ilayer) );

        // the previous layer info is needed to calculate parameters of current layer
        const auto& previousLayer = this->database.at(ilayer-1);

        //calculate height_z0, radius_sw_z0
        {
            double h  = previousLayer.height_z0;
            double ru = previousLayer.radius_fuw_z0;
            double rd = previousLayer.radius_fdw_z0;

            if(0 == Get_nsuperlayer_minus_1(ilayer))
                layer_info.height_z0 = h*ru/rd;
            else
                layer_info.height_z0 = TMath::TwoPi()*ru/(0.5*layer_info.nwires - TMath::Pi());

            layer_info.radius_sw_z0 = 0.5*layer_info.height_z0 + ru;
        }

        //calculate radius_fdw_z0, radius_fuw_z0, width_z0
        layer_info.radius_fdw_z0 = previousLayer.radius_fuw_z0;
        layer_info.radius_fuw_z0 = previousLayer.radius_fuw_z0 + layer_info.height_z0;
        layer_info.width_z0 = TMath::TwoPi()*layer_info.radius_sw_z0/(0.5*layer_info.nwires);

        // according to expert prescription, width_z0 == height_z0
        if(fabs(layer_info.width_z0 - layer_info.height_z0)>1e-4)
            throw std::runtime_error("fabs(l.width_z0 - l.height_z0)>1e-4");



        this->database.emplace(ilayer, layer_info);
    }

    std::cout << "\t+ Total size of DCH database = " << database.size() << std::endl;
    return;
}

inline void DCH_info_struct::Show_DCH_info_database(std::ostream & oss) const
{
    oss << "\n";
    oss << "Global parameters of DCH:\n";
    oss << "\tGas, half length/mm = " << Lhalf/dd4hep::mm << '\n';
    oss << "\tGas, radius in/mm  = " << rin/dd4hep::mm << '\n';
    oss << "\tGas, radius out/mm = " << rout/dd4hep::mm<< '\n';
    oss << "\tGuard, radius in(z=0)/mm  = " << guard_inner_r_at_z0/dd4hep::mm << '\n';
    oss << "\tGuard, radius out(z=L/2)/mm = " << guard_outer_r_at_zL2/dd4hep::mm << '\n';
    oss << "\n";
    oss << "\tTwist angle (2*alpha) / deg = " << twist_angle/dd4hep::deg << '\n';
    oss << "\n";
    oss << "\tN superlayers = " << nsuperlayers << '\n';
    oss << "\tN layers per superlayer = " << nlayersPerSuperlayer << '\n';
    oss << "\tN layers = " << nlayers << '\n';
    oss << "\n";
    oss << "\tN cells layer1 = " << ncell0 << '\n';
    oss << "\tN cells increment per superlayer = " << ncell_increment << '\n';
    oss << "\tN cells per sector = " << ncell_per_sector << '\n';
    oss << "\n";
    oss << "Layer parameters of DCH:\n";
    oss
            << "\t" << "layer"
            << "\t" << "nwires"
            << "\t" << "height_z0/mm"
            << "\t" << "width_z0/mm"
            << "\t" << "radius_fdw_z0/mm"
            << "\t" << "radius_sw_z0/mm"
            << "\t" << "radius_fuw_z0/mm"
            << "\t" << "stereoangle_z0/deg"
            << "\t" << "Pitch_z0/mm"
            << "\t" << "radius_sw_zLhalf/mm"
            << "\t" << "WireLength/mm"
            << "\n" << std::endl;

    if( this->IsDatabaseEmpty() )
    {
        oss << "\nDatabase empty\n";
        return;
    }

    for(const auto& [nlayer, l]  : database )
    {
        oss
                << "\t" << l.layer
                << "\t" << l.nwires
                << "\t" << l.height_z0/dd4hep::mm
                << "\t" << l.width_z0/dd4hep::mm
                << "\t" << l.radius_fdw_z0/dd4hep::mm
                << "\t" << l.radius_sw_z0/dd4hep::mm
                << "\t" << l.radius_fuw_z0/dd4hep::mm
                << "\t" << l.StereoSign()*this->stereoangle_z0(l.radius_sw_z0)/dd4hep::deg
                << "\t" << l.Pitch_z0(l.radius_sw_z0)/dd4hep::mm
                << "\t" << this->Radius_zLhalf(l.radius_sw_z0)/dd4hep::mm
                << "\t" << this->WireLength(l.layer,l.radius_sw_z0)/dd4hep::mm
                << "\n" << std::endl;
    }
    return;
}
}} // end namespace dd4hep::rec::

#endif // DDREC_DCH_INFO_H
