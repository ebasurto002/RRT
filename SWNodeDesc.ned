//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple swNodeTx
{
    parameters:
        @display("i=block/routing");
        double timeout = default(1.0);
    gates:
        input in;
        input inSnd;
        output out;
}
simple swNodeRx
{
    parameters:
        @display("i=block/routing");       
    gates:
        input in;
        output out;
}

simple injector
{
    parameters:
        @display("i=block/source");
        int n_paquetes = default(1000);
    gates:
        output outSnd;
}

network swNetwork
{
    types:
        channel C extends ned.DatarateChannel {
            datarate = 96000bps;
            delay=100ms;
            per= 0.5;
        }
    submodules:
        swSndr: swNodeTx;
        swRcvr: swNodeRx;
        src: injector;
    connections:
        src.outSnd --> swSndr.inSnd;
        swSndr.out --> C --> swRcvr.in;
        swSndr.in <-- C <-- swRcvr.out;
}
