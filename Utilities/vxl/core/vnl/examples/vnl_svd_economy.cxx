#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_sample.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_svd_economy.h>

int main()
{
  {
    vnl_matrix<double> M( 10, 4 );
    for (unsigned int i=0 ; i < M.size(); ++i) {
      M.data_block()[i] = vnl_sample_uniform(-1,1);
    }

    vnl_svd<double> svd( M );
    vnl_svd_economy<double> svd_e( M );

    vnl_matlab_print( vcl_cerr, svd.V() );
    vcl_cerr << vcl_endl;
    vnl_matlab_print( vcl_cerr, svd_e.V() );
    vcl_cerr << vcl_endl << vcl_endl;

    vnl_matlab_print( vcl_cerr, svd.W().diagonal() );
    vcl_cerr << vcl_endl;
    vnl_matlab_print( vcl_cerr, svd_e.lambdas() );

    vcl_cerr << "\n( svd.V() - svd_e.V() ).fro_norm() = " << ( svd.V() - svd_e.V() ).fro_norm()
             << "\n( svd.W().diagonal() - svd_e.lambdas() ).two_norm() = "
             << ( svd.W().diagonal() - svd_e.lambdas() ).two_norm() << vcl_endl;
  }

  {
    vnl_matrix<double> N( 2000, 12 );
    for (unsigned int i=0 ; i < N.size(); ++i)
      N.data_block()[i] = vnl_sample_uniform(-1,1);

    vul_timer timer;
    for (int i=0; i < 1000; ++i)
      vnl_svd<double> svd( N );

    int t1 = timer.user();
    timer.mark();
    for (int i=0; i < 1000; ++i)
      vnl_svd_economy<double> svd_e( N );

    int t2 = timer.user();

    vcl_cerr << "time for 1000*svd(1000x10) : vnl_svd = " << t1 << " msec, "
             << "vnl_svd_economy = " << t2 << " msec.\n";
  }

  return 0;
}
