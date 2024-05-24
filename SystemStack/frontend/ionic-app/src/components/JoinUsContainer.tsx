import { useContext } from 'react';
import './ExploreContainer.css';
import ApiWrapper from '../support/APIWrapper';

const JoinUsContainer: React.FC = () => {
  return (
    <div className="container">
      <strong>Since 2024</strong>
      <p>Value your privacy? <a href={ApiWrapper.backendURI + "login"}>Join Us.</a></p>
    </div>
  );
};

export default JoinUsContainer;
