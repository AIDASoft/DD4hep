#ifndef DDRec_Vector2D_h
#define DDRec_Vector2D_h 1

namespace DD4hep { namespace DDRec {
  
  /** Simple 2D vector helper class; moved out of ISurface definition.
   *
   */
  
    class Vector2D
    {
        public: 
        Vector2D() : _u(0.), _v(0.) {}
        Vector2D(double u_val, double v_val) : _u(u_val), _v(v_val) {}
        
        double operator[](unsigned i) const
        {
            return i == 0 ? _u : _v ;
        }
        
        double u() const  { return _u ; }
        
        double v() const  { return _v ; }
        
        double& u() { return _u; }
        double& v() { return _v; }
        
        private:
            double _u, _v ;

    };
  } } // namespace

#endif 
