#ifndef DDSurfaces_Vector2D_h
#define DDSurfaces_Vector2D_h 1

namespace DDSurfaces {
  
  /** Simple 2D vector helper class; moved out of ISurface definition.
   *
   */
  
    class Vector2D
    {
        public: 
        Vector2D() : _u(0.), _v(0.) {}
        Vector2D(double u, double v) : _u(u), _v(v) {}
        
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
} // namespace

#endif 
