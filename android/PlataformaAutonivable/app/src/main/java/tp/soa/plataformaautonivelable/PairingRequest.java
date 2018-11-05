package tp.soa.plataformaautonivelable;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

public class PairingRequest extends BroadcastReceiver {
    public PairingRequest() {
        super();
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction().equals("android.bluetooth.device.action.PAIRING_REQUEST")) {
            try {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                //int pin=intent.getIntExtra("android.bluetooth.device.extra.PAIRING_KEY", 0);
                int pin = 123456;
                byte[] pinBytes;
                pinBytes = (""+pin).getBytes("UTF-8");
                device.setPin(pinBytes);
                //setPairing confirmation if neeeded
                //device.setPairingConfirmation(true);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}