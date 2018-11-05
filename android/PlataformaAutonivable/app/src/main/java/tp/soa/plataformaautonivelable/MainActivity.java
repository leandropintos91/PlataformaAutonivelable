package tp.soa.plataformaautonivelable;

import java.text.DecimalFormat;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import java.lang.reflect.Method;
import android.content.IntentFilter;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.widget.Toast;
import java.util.ArrayList;
import java.util.Set;

public class MainActivity extends Activity
{

    public BluetoothAdapter mBluetoothAdapter;
    private TextView      textViewEstado;
    private Button        buttonConectar;
    private ProgressDialog mProgressDlg;

    private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();



    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        buttonConectar = (Button) findViewById(R.id.buttonConectar);
        textViewEstado = (TextView) findViewById(R.id.textViewEstado);


        mProgressDlg = new ProgressDialog(this);



        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        buttonConectar.setOnClickListener(new OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(intent, 1000);
            }
        });

       IntentFilter filter = new IntentFilter(
                "android.bluetooth.device.action.PAIRING_REQUEST");

        registerReceiver(
                new PairingRequest(), filter);

        IntentFilter filter2 = new IntentFilter();

        filter2.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter2.addAction(BluetoothDevice.ACTION_FOUND);
        filter2.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter2.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter2.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        registerReceiver(mReceiver, filter2);

        if (mBluetoothAdapter == null) {
            textViewEstado.setText("Bluetooth no soportado.");
            buttonConectar.setEnabled(false);
        } else {
            if (!mBluetoothAdapter.isEnabled()) {
                textViewEstado.setText("Bluetooth apagado.");
            } else
            {
                textViewEstado.setText("Bluetooth encendido.");
            }
        }

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == 1000) {
            int MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION = 1;
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION);
            mBluetoothAdapter.startDiscovery();
            textViewEstado.setText("Conectando...");
            mProgressDlg.setMessage("Conectando...");
            mProgressDlg.setCancelable(false);
            mProgressDlg.show();
        }
    }

    private void pairDevice(BluetoothDevice device) {
        try {
            Method method = device.getClass().getMethod("createBond", (Class[]) null);
            method.invoke(device, (Object[]) null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void showToast(String message) {
        Toast.makeText(MainActivity.this, message, Toast.LENGTH_SHORT).show();
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {

            //Atraves del Intent obtengo el evento de Bluethoot que informo el broadcast del SO
            String action = intent.getAction();

            if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action))
            {
                //Creo la lista donde voy a mostrar los dispositivos encontrados
                mDeviceList = new ArrayList<BluetoothDevice>();

            }

            if (BluetoothDevice.ACTION_FOUND.equals(action))
            {
                //Se lo agregan sus datos a una lista de dispositivos encontrados
                BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                if(device.getName() != null && device.getName().equals("PlataformaAutonivelable"))
                    mDeviceList.add(device);
                Log.d("NOMBRE", "NAME" + device.getName());
            }


            if (BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action))
            {
                //Obtengo los parametro, aplicando un Bundle, que me indica el estado del Bluethoot
                final int state = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
                final int prevState = intent.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE, BluetoothDevice.ERROR);

                //se analiza si se puedo emparejar o no
                if (state == BluetoothDevice.BOND_BONDED && prevState == BluetoothDevice.BOND_BONDING)
                {
                    BluetoothDevice dispositivo = mDeviceList.get(0);

                    mProgressDlg.dismiss();
                    String direccionBluethoot = dispositivo.getAddress();
                    Intent i = new Intent(MainActivity.this, activity_comunicacion.class);
                    i.putExtra("Direccion_Bluethoot", direccionBluethoot);

                    startActivity(i);

                }
                else if (state == BluetoothDevice.BOND_NONE && prevState == BluetoothDevice.BOND_BONDED) {

                }
            }


            if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action))
            {
                //Se lo agregan sus datos a una lista de dispositivos encontrados

                // mDeviceList = intent.getParcelableArrayListExtra("device.list");
                if(mDeviceList.size() > 0) {
                    Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
                    Boolean estaEmparejado = false;
                    for (BluetoothDevice bt : pairedDevices)
                        if (bt.getName().equals(mDeviceList.get(0).getName()))
                            estaEmparejado = true;

                    if (!estaEmparejado)
                        pairDevice(mDeviceList.get(0));
                    else {
                        mProgressDlg.dismiss();
                        String direccionBluethoot = mDeviceList.get(0).getAddress();
                        Intent i = new Intent(MainActivity.this, activity_comunicacion.class);
                        i.putExtra("Direccion_Bluethoot", direccionBluethoot);
                        startActivity(i);
                    }
                }else
                {
                    mBluetoothAdapter.startDiscovery();
                }
            }
        }
    };






    @Override
    protected void onStop()
    {

        //Parar_Sensores();

        super.onStop();
    }

    @Override
    protected void onDestroy()
    {
        //Parar_Sensores();
        mBluetoothAdapter.disable();
        super.onDestroy();
    }

    @Override
    protected void onPause()
    {
        //Parar_Sensores();

        super.onPause();
    }

    @Override
    protected void onRestart()
    {
        //Ini_Sensores();

        super.onRestart();
    }

    @Override
    protected void onResume()
    {
        super.onResume();

       // Ini_Sensores();
    }

}
