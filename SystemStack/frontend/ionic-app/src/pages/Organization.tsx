import { useContext } from "react";
import { AuthContext, URIContext } from "../App";
import { IonContent } from "@ionic/react";

const Organization: React.FC = () => {
    const logged = useContext(AuthContext);
    const backendURI = useContext(URIContext);
    return (
        <IonContent>
            
        </IonContent>
    );
};

export default Organization;