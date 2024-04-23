import { IonIcon } from '@ionic/react';
import './ExploreContainer.css';
import { alertCircleOutline } from 'ionicons/icons';

interface ContainerProps {
  uri: string;
}

const AccessDeniedContainer: React.FC<ContainerProps> = ({ uri }) => {
  return (
    <div className="container" style={{ textAlign: 'center' }}>
      <div className="row">
        <IonIcon aria-hidden="true" icon={alertCircleOutline} style={{ fontSize: '4em'}}/>
      </div>
      <strong>Access Denied!</strong>
      <p>Please, <a href={uri + "login"}>login</a> to access this content.</p>
    </div>
  );
};

export default AccessDeniedContainer;
