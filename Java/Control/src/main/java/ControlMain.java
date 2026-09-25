import control.ControlLayer;

import java.io.IOException;
import java.sql.SQLException;

public class ControlMain {
    public static void main(String[] args) throws IOException, SQLException, InterruptedException {
        ControlLayer control = new ControlLayer();
        while (true) {
            control.controlPipelineStep();
        }
    }
}
